/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javaapplication3;


/**
 *
 * @author alex
 */
public class JavaApplication3 {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {
        
        ConfigReader cnf_reader = new ConfigReader();
        try {
            cnf_reader.parse("config.xml");
        }catch (Exception e){
            System.out.println("Exeption: " + e.getMessage());
        }

        //----------------------------------------------------------------------
        DataXmlWorker dataxmlworker = new DataXmlWorker();
        dataxmlworker.setFileParams(cnf_reader.getCnfParam("exchange", "file"), 
                                    Integer.parseInt(cnf_reader.getCnfParam("exchange", "check_period")));       
        //----------------------------------------------------------------------
        ModBusWorker modbus_worker = new ModBusWorker();
        modbus_worker.setComPortParams(cnf_reader.getCnfParam("comport", "name"), 
                             Integer.parseInt(cnf_reader.getCnfParam("comport", "speed")));
        modbus_worker.setModBusParams(Integer.parseInt(cnf_reader.getCnfParam("modbus", "client_start_id")), 
                            Integer.parseInt(cnf_reader.getCnfParam("modbus", "client_max_count")), 
                            Integer.parseInt(cnf_reader.getCnfParam("modbus", "data_start_address")),
                            Integer.parseInt(cnf_reader.getCnfParam("modbus", "client_data_address")));
        
        modbus_worker.setDataXmlWorker(dataxmlworker);
        
        //dataxmlworker.test();
        
        try {
            modbus_worker.init();
            long curtime = System.currentTimeMillis();
            while (true){
                //dataxmlworker.test();
                if (System.currentTimeMillis() - curtime > 1000){
                    curtime = System.currentTimeMillis();
                    modbus_worker.updateData();
                }
                //System.out.println("modbus_worker updateData done");
            }
        }catch (Exception e){
            System.out.println("Exeption in main: " + e.getMessage());
        }finally{
            modbus_worker.destroy();
        }

    }
       
       
}
