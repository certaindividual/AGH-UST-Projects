package app.model;

import java.util.List;
import java.util.Date;

public class DataPointList {

    private String path;

    private List<DataPoint> dataPoints;

    public DataPointList(List<DataPoint> dataPoints) {
        this.dataPoints = dataPoints;
    }

    public DataPoint get(int i){
        return this.dataPoints.get(i);
    }

    public int getIndexOfData(Date date) {
        for(int i =0; i<this.dataPoints.size();i++) {
            if (this.dataPoints.get(i).getDate().compareTo(date) == 0) {
                return i;
            }
        }
        return -1;
    }

    public int size(){
        return this.dataPoints.size();
    }

    public void setDataPoints(List<DataPoint> dataPoints) {
        this.dataPoints = dataPoints;
    }

    public List<DataPoint> getDataPoints() {
        return dataPoints;
    }

    public String getPath() {
        return path;
    }

    public void addDataPointToList(DataPoint dp) {
        dataPoints.add(dp);
    }
}
