/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javaapplication3;

import java.util.Iterator;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.ShortBuffer;
import org.dom4j.Element;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.io.SAXReader;
import org.dom4j.io.XMLWriter;
import java.util.logging.Level;
import java.util.logging.Logger;
/**
 *
 * @author blobby
 */
public class DataXmlWorker {
    
    // FUNCTIONS
    
    public void setFileParams(String file_name, int check_period, int mem_shift){
        file_name_ = file_name;
        check_period_ = check_period;
        mem_shift_ = mem_shift;
        last_change_timestamp_ = getChangeTimeStamp();
        
        //System.out.println("getChangeTimeStamp() = " + last_change_timestamp_);
    }
    
    public ShortBuffer upDateData (ShortBuffer mem_data){
        
        if (last_change_timestamp_ < getChangeTimeStamp()){
            //System.out.println("some new data in XML");
            ShortBuffer cur_data = getXmlData(mem_data);
            setXmlData(mem_data);
            last_change_timestamp_ = getChangeTimeStamp();
            return cur_data;
        }
        
        if ((System.currentTimeMillis() - last_change_timestamp_) < check_period_){
            //System.out.println("NO upDateData: " + (System.currentTimeMillis() - last_change_timestamp_));
            return mem_data;
        }
        
        //System.out.println("in upDateData");
        //System.out.println("No new data in XML");
        setXmlData(mem_data);
        last_change_timestamp_ = getChangeTimeStamp();
        return mem_data;
    }
    
    private ShortBuffer getXmlData(ShortBuffer mem_data){
        
        ShortBuffer exitData = ShortBuffer.allocate(mem_data.limit());
        exitData.put(mem_data.array());
        
        SAXReader reader = new SAXReader();

        File f = new File(file_name_);

        if (!f.exists()){
            System.out.println("no data file " + file_name_);
        }
        
        try {
            Document document = reader.read(f);
            
            Element root = document.getRootElement().element("map");
            
            // iterate through child elements of root
            for ( Iterator i = root.elementIterator(); i.hasNext(); ) {
                Element element = (Element) i.next();
                
                //System.out.println("addr[ " + element.attributeValue("addr") + "] = " + element.attributeValue("value"));
                
                int mem_adr = Integer.parseInt(element.attributeValue("addr")) - mem_shift_;
                if (mem_adr < mem_data.limit())
                    exitData.put(mem_adr, (short) Integer.parseInt(element.attributeValue("value")));
            }

            return exitData;
        } catch (DocumentException ex) {
            System.out.println("Exeption: " + ex.getMessage());
            Logger.getLogger(DataXmlWorker.class.getName()).log(Level.SEVERE, null, ex);
        }

        return exitData;
    }
    
    private void setXmlData(ShortBuffer mem_data){
        SAXReader reader = new SAXReader();

        File f = new File(file_name_);

        if (!f.exists()){
            System.out.println("no data file " + file_name_);
        }
        
        try {
            Document document = reader.read(f);
            
            Element root = document.getRootElement().element("map");

            // iterate through child elements of root
            for ( Iterator i = root.elementIterator(); i.hasNext(); ) {
                Element element = (Element) i.next();
           
                //System.out.println("addr[ " + element.attributeValue("addr") + "] = " + element.attributeValue("value"));
                
                int mem_adr = Integer.parseInt(element.attributeValue("addr")) - mem_shift_;
                //System.out.println("mem_adr = " + mem_adr);
                
                if (mem_adr < mem_data.limit()){
                    int data = mem_data.get(mem_adr);
                    if (data < 0 )
                        data += 65536;
                    //System.out.println("in put attr to xml int = " + data);
                    element.attribute("value").setValue(Integer.toString(data));
                }
            }
            
//            XMLWriter writer = new XMLWriter( new FileWriter( "output.xml" ));
            XMLWriter writer = new XMLWriter( new FileWriter( file_name_ ));
            writer.write( document );
            writer.close();
            
        } catch (DocumentException ex) {
            System.out.println("Exeption: " + ex.getMessage());
            Logger.getLogger(DataXmlWorker.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            System.out.println("Exeption: " + ex.getMessage());
            Logger.getLogger(DataXmlWorker.class.getName()).log(Level.SEVERE, null, ex);
        }
                
    }
    
    private long getChangeTimeStamp (){
        return new File(file_name_).lastModified();
    }
    
    // FIELDS
    private String file_name_;
    private int check_period_;
    private long last_change_timestamp_;
    private int mem_shift_;
    
    
    
}
