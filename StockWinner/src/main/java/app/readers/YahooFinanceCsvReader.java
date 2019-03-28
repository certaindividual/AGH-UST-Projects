package app.readers;

import java.io.*;
import java.math.BigDecimal;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;

import app.controllers.AppController;
import app.exceptions.InvalidContentException;
import app.model.DataPoint;
import app.model.DataPointList;
import javafx.scene.control.Alert;

public class YahooFinanceCsvReader extends FormatReader {

     public DataPointList getDataPointList(String path) throws InvalidContentException {
        List<DataPoint> pointList = new ArrayList<DataPoint>();

        String line;
        String cvsSplitBy = ",";
        SimpleDateFormat ft = new SimpleDateFormat ("yyyy-MM-dd");

        try {
            BufferedReader br = new BufferedReader(new FileReader(path));
            if(!br.readLine().split(cvsSplitBy)[0].equals("Date")) throw new InvalidContentException();
            
             while ((line = br.readLine()) != null) {
                String[] data = line.split(cvsSplitBy);
                DataPoint dataPoint = new DataPoint();
                dataPoint.setDate(ft.parse(data[0]));
                dataPoint.setPrice(new BigDecimal(data[1]));
                pointList.add(dataPoint);
            }

        } catch (IOException e) {

        } catch (ParseException e) {
            throw new InvalidContentException();
        }

         return new DataPointList(pointList);
    }
}
