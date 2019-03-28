package app.readers;

import app.exceptions.InvalidContentException;
import app.model.DataPoint;
import app.model.DataPointList;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.math.BigDecimal;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class NbpJsonReader extends FormatReader {


    public DataPointList getDataPointList(String path) throws InvalidContentException {

        List<DataPoint> pointsList = new ArrayList<DataPoint>();
        SimpleDateFormat ft = new SimpleDateFormat ("yyyy-MM-dd");

        try{
            Object obj = new JSONParser().parse(new FileReader(path));
            if(obj instanceof List<?>){         //dla zlota
                Iterator iterator = ((List) obj).iterator();
                while (iterator.hasNext()){
                        JSONObject jo = (JSONObject) iterator.next();
                        DataPoint dataPoint = new DataPoint();
                        dataPoint.setDate(ft.parse(jo.get("data").toString()));
                        dataPoint.setPrice(new BigDecimal(jo.get("cena").toString()));
                        pointsList.add(dataPoint);
                }

            } else {    //dla walut
                JSONObject jo = (JSONObject) obj;
                JSONArray ja = (JSONArray) jo.get("rates");
                if(ja == null) throw new InvalidContentException();
                Iterator itr2 = ja.iterator();
                Iterator<Map.Entry> itr1 = jo.entrySet().iterator();

                while (itr2.hasNext()) {
                    DataPoint dataPoint = new DataPoint();
                    itr1 = ((Map) itr2.next()).entrySet().iterator();
                    while (itr1.hasNext()) {
                        Map.Entry pair = itr1.next();

                        if (pair.getKey().equals("effectiveDate")) {
                            dataPoint.setDate(ft.parse((String) pair.getValue()));
                        } else if (pair.getKey().equals("data")) {
                            dataPoint.setDate(ft.parse((String) pair.getValue()));
                        }
                        if (pair.getKey().equals("mid")) {
                            dataPoint.setPrice(new BigDecimal(pair.getValue().toString()));
                        } else if (pair.getKey().equals("cena")) {
                            dataPoint.setPrice(new BigDecimal(pair.getValue().toString()));
                        }

                    }
                    pointsList.add(dataPoint);
                }
            }

        } catch (FileNotFoundException e) {


        } catch (IOException e) {

        } catch (java.text.ParseException e) {

        } catch (ParseException e) {
            throw new InvalidContentException();
        }

        return new DataPointList(pointsList);
    }
}
