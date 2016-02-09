/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javaapplication3;


import java.util.Iterator;
import java.io.File;
import java.nio.ShortBuffer;
import org.dom4j.Element;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.io.SAXReader;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 *
 * @author blobby
 */
public class TimeDataXmlWorker {
    
    public void setFileParams(String file_name, int mem_shift){
        file_name_ = file_name;
        mem_shift_ = mem_shift;
        last_change_timestamp_ = getChangeTimeStamp();
        
        System.out.println("getChangeTimeStamp() to time file = " + last_change_timestamp_);
        System.out.println("file_name = " + file_name);
    }
    
    public ShortBuffer upDateData (ShortBuffer mem_data){
        
        //System.out.println("in update last_change_timestamp_ = " + last_change_timestamp_);
        //System.out.println("in update getChangeTimeStamp()   = " + getChangeTimeStamp());
        
        if (last_change_timestamp_ < getChangeTimeStamp()){
            try{
                //System.out.println("some new data in XML");
                ShortBuffer cur_data = getXmlData(mem_data);
                last_change_timestamp_ = getChangeTimeStamp();
                return cur_data;
            }catch(Exception ex){
                System.out.println("Exception in time data update: " + ex.getMessage());
            }
            
        }
        
        
        return mem_data;
    }
    
    
    private ShortBuffer getXmlData(ShortBuffer mem_data){
        
        ShortBuffer exitData = ShortBuffer.allocate(mem_data.limit());
        exitData.put(mem_data.array());
        
        //for (char ii = 0; ii < exitData.limit(); ii++){
        //    System.out.println("exitData[" + ii + "]=" + exitData.get(ii));
        //}
        
        SAXReader reader = new SAXReader();

        File f = new File(file_name_);
        
        if (!f.exists()){
            System.out.println("no time data file " + file_name_);
        }
        
        try {
            Document document = reader.read(f);
            
            Element root = document.getRootElement();
            
            // iterate through child elements of root
            for ( Iterator i = root.elementIterator(); i.hasNext(); ) {
                Element element = (Element) i.next();

                //System.out.println("addr[ " + element.attributeValue("addr") + "] = " + element.attributeValue("value"));
                
                int mem_adr = Integer.parseInt(element.attributeValue("addr")) - mem_shift_;
                if (mem_adr < mem_data.limit())
                    exitData.put(mem_adr, Short.parseShort(element.attributeValue("value")));

            }

            return exitData;
        } catch (DocumentException ex) {
            System.out.println("Exeption: " + ex.getMessage());
            Logger.getLogger(DataXmlWorker.class.getName()).log(Level.SEVERE, null, ex);
        }

        return exitData;
    }
    
    private long getChangeTimeStamp (){
        return new File(file_name_).lastModified();
    }
    
    // FIELDS
    private String file_name_;
    private long last_change_timestamp_;
    private int mem_shift_;
    
}
