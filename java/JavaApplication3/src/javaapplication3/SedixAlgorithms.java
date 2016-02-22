/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javaapplication3;

import java.nio.ShortBuffer;
/**
 *
 * @author Alex
 */
public class SedixAlgorithms {
    
    public ShortBuffer aplyAlgo(ShortBuffer mem_data){
    
        ShortBuffer exitData = ShortBuffer.allocate(mem_data.limit());
            exitData.put(mem_data.array());
            
            
            
        // пиши свой код здесь. 
        // работай только с буфером ExitData. его размерность short, то есть 2 байта (один регистр)
        // основные команды для тебя это вытащить из буфера по заданному адресу 
        // short x = exitData.get(position);
        // и записать значение в буфер по заданному адресу
        // exitData.put(position, value);
        // размер массива определяется количеством модулей, которое определено в конфигурации
        // весь этот массив сразу после обработки будет разослан всем модулям
        // следовательно первые 4 регистра = время
        // следующие 4 принадлежат модулю
        // после того, как вытащил данные из буфера, работаешь с ними как обычно
        // никаких подводных камней и специальных операторов нет
            
        return exitData;    
    }
}
