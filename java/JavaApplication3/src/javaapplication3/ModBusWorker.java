/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javaapplication3;

import com.serotonin.io.serial.SerialParameters;
import com.serotonin.modbus4j.ModbusFactory;
import com.serotonin.modbus4j.ModbusMaster;
import com.serotonin.modbus4j.code.DataType;
import com.serotonin.modbus4j.code.RegisterRange;
import com.serotonin.modbus4j.exception.ErrorResponseException;
import com.serotonin.modbus4j.exception.ModbusInitException;
import com.serotonin.modbus4j.exception.ModbusTransportException;

import com.serotonin.modbus4j.msg.ReadHoldingRegistersRequest;
import com.serotonin.modbus4j.msg.ReadHoldingRegistersResponse;
import com.serotonin.modbus4j.msg.ModbusRequest;
import com.serotonin.modbus4j.msg.ModbusResponse;
import com.serotonin.modbus4j.msg.WriteRegistersRequest;
import com.serotonin.modbus4j.msg.WriteRegistersResponse;
import com.serotonin.util.queue.ByteQueue;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author blobby
 */
public class ModBusWorker {
    
    
    public void init (){
        
        ModbusFactory factory = new ModbusFactory();
        SerialParameters params = new SerialParameters();
            params.setCommPortId(port_name_);
            params.setBaudRate(port_speed_);
            
            
            params.setDataBits(8);
            params.setStopBits(1);
            params.setParity(0);
        master_ = factory.createRtuMaster(params);
            master_.setTimeout(timeout_ms_);
            master_.setRetries(0);
            
 
        // Don't start if the RTU master can't be initialized.
        try {
            master_.init();
        } catch (ModbusInitException e) {
          System.out.println( "Modbus Master Init Error: " + e.getMessage());
          return;
        }

        sedix_alg = new SedixAlgorithms();
    }
    
    // FUNCTIONS update data
    public void updateData (){
        
        if(!master_.isInitialized()){
            System.out.println("master not init");
            init();
            return;
        }
        
        long start = System.currentTimeMillis();
        
        //System.out.println("Gathering data from slaves");

        for (int i = client_start_id_; i <= client_max_count_; i++){
            // TODO FOR DEBUG
            int addr = i;
            try{
                ReadHoldingRegistersResponse res = (ReadHoldingRegistersResponse)master_.send(new ReadHoldingRegistersRequest(
                                addr, 
                                client_data_address_ - 1, 
                                reg_count_per_client_to_read_));

                map_.position(i * reg_count_per_client_);
                map_.put(ByteBuffer.wrap(res.getData()).asShortBuffer());

           }catch (ModbusTransportException ex){
                System.out.println("ModbusTransportException: " + ex.getMessage());
                Logger.getLogger(ModBusWorker.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        System.out.println("Gathering time: " + (System.currentTimeMillis() - start) + "ms");
        
        //System.out.println("Update xml");
        
        //for (char ii = 0; ii < map_.limit(); ii++){
        //    System.out.println("map[" + ii + "]=" + map_.get(ii));
        //}

        
        map_ = time_data_xml_worker_.upDateData(map_);
//         for (int ii = 0; ii < map_.limit(); ii++){
//            System.out.println("map+t[" + ii + "]=" + map_.get(ii));
//        }
       map_ = data_xml_worker_.upDateData(map_);
//         for (int iii = 0; iii < map_.limit(); iii++){
//            System.out.println("map+time+t[" + iii + "]=" + map_.get(iii));
//        }
        
        map_ = sedix_alg.aplyAlgo(map_);
//         for (int iiii = 0; iiii < map_.limit(); iiii++){
//            System.out.println("map+time+alg+t[" + iiii + "]=" + map_.get(iiii));
//        }


//System.out.println("Update xml finished");
        
        //System.out.println("Send Braodcast data");
        
        
        
        // TODO need set broadcast address
        try {
            master_.send(new WriteRegistersRequest(0, data_start_address_ - 1, map_.array()));            
        } catch (ModbusTransportException ex) {
            System.out.println("try send Broadcast exception: " + ex.getMessage());
            //Logger.getLogger(ModBusWorker.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        //System.out.println("Send Broadcast data complete");
        System.out.println("Full Gathering time: " + (System.currentTimeMillis() - start) + "ms");
    }

    // FUNCTIONS TO SET FIELDS
    public void setModBusParams (   int client_start_id, 
                                    int client_max_count,
                                    int data_start_address,
                                    int client_data_address,
                                    int timeout_ms){
        client_start_id_ = client_start_id;
        client_max_count_ = client_max_count;
        data_start_address_ = data_start_address;
        reg_count_per_client_ = 4;
        reg_count_per_client_to_read_ = 2;
        client_data_address_ = client_data_address;
        timeout_ms_ = timeout_ms;
        
        map_ = ShortBuffer.allocate((client_max_count_ + 1 ) * reg_count_per_client_ );
        
    }
    
    public void setComPortParams (   String port_name, 
                                     int port_speed){
        port_speed_ = port_speed;
        port_name_ = port_name;
    }
    
    public void setDataXmlWorker(DataXmlWorker data_xml_worker){
        data_xml_worker_ = data_xml_worker;
    }

    public void setTimeDataXmlWorker(TimeDataXmlWorker time_data_xml_worker){
        time_data_xml_worker_ = time_data_xml_worker;
    }
    
    public void destroy(){
        master_.destroy();
    }
    
    // FIELDS
    private int client_start_id_; 
    private int client_max_count_; 
    private int data_start_address_;
    private int reg_count_per_client_;
    private int reg_count_per_client_to_read_;
    private int client_data_address_; 
    private int timeout_ms_;
    
    private String port_name_;
    private int port_speed_;
    
    private ModbusMaster master_;
    
    private ShortBuffer map_;
    
    private DataXmlWorker data_xml_worker_;
    private TimeDataXmlWorker time_data_xml_worker_;
    
    private SedixAlgorithms sedix_alg;
}
