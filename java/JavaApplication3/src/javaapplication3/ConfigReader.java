/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javaapplication3;

import java.io.File;
import org.dom4j.Element;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.io.SAXReader;

/**
 *
 * @author blobby
 */
public class ConfigReader {
    
    // FUNCTION 
    public void parse(String filename) throws DocumentException {
        
        SAXReader reader = new SAXReader();

        File f = new File(filename);

        if (!f.exists()){
            System.out.println("no file " + filename);
        }

        document_ = reader.read(f);
        root_ = document_.getRootElement();
    }

    // FUNCTION
    public String getCnfParam(String tag_name, String attr_name){
        return root_.element(tag_name).attribute(attr_name).getStringValue();
    }
    // FIELDS
    private Document document_;
    private Element root_;

}
