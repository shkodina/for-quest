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
        String config_path = "config.xml";
        
        if (args.length == 0){
            System.out.println("Usage: master config_path");
            //System.exit(1);
        }else{
            config_path = args[0];
        }
        
         
        
        ConfigReader cnf_reader = new ConfigReader();
        try {
            cnf_reader.parse(config_path);
        }catch (Exception e){
            System.out.println("Exeption: " + e.getMessage());
        }

        //----------------------------------------------------------------------
        DataXmlWorker dataxmlworker = new DataXmlWorker();
        dataxmlworker.setFileParams(cnf_reader.getCnfParam("exchange", "map_file_from_unit"), 
                                    cnf_reader.getCnfParam("exchange", "map_file_to_unit"),
                                    Integer.parseInt(cnf_reader.getCnfParam("exchange", "check_period")),
                                    Integer.parseInt(cnf_reader.getCnfParam("modbus", "data_start_address")));       
        //----------------------------------------------------------------------
        TimeDataXmlWorker timedataxmlworker = new TimeDataXmlWorker();
        timedataxmlworker.setFileParams(cnf_reader.getCnfParam("exchange", "time_file"), 
                                    Integer.parseInt(cnf_reader.getCnfParam("modbus", "data_start_address")));       
        //----------------------------------------------------------------------
        ModBusWorker modbus_worker = new ModBusWorker();
        modbus_worker.setComPortParams(cnf_reader.getCnfParam("comport", "name"), 
                             Integer.parseInt(cnf_reader.getCnfParam("comport", "speed")));
        modbus_worker.setModBusParams(Integer.parseInt(cnf_reader.getCnfParam("modbus", "client_start_id")), 
                            Integer.parseInt(cnf_reader.getCnfParam("modbus", "client_max_count")), 
                            Integer.parseInt(cnf_reader.getCnfParam("modbus", "data_start_address")),
                            Integer.parseInt(cnf_reader.getCnfParam("modbus", "client_data_address")));
        
        modbus_worker.setDataXmlWorker(dataxmlworker);
        modbus_worker.setTimeDataXmlWorker(timedataxmlworker);
        
        int poll_period = Integer.parseInt(cnf_reader.getCnfParam("modbus", "poll_period"));
        try {
            modbus_worker.init();
            long curtime = System.currentTimeMillis();
            while (true){
                //dataxmlworker.test();
                if (System.currentTimeMillis() - curtime > poll_period){
                    curtime = System.currentTimeMillis();
                    modbus_worker.updateData();
                }
                Thread.sleep(50);
                //System.out.println("modbus_worker updateData done");
            }
        }catch (Exception e){
            System.out.println("Exeption in main: " + e.getMessage());
        }finally{
            modbus_worker.destroy();
        }

    }
       
       
}
