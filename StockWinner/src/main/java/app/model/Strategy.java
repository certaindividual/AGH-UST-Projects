package app.model;

import app.exceptions.NoValidDateFoundException;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class Strategy implements IStrategyComponent{

    private int daysPrior;
    private Date fromDate, toDate;
    private BigDecimal percent;
    private StrategyEnums.Change action;
    private DataPointList dataPointList;

    public Strategy(int daysPrior, BigDecimal percent, StrategyEnums.Change action, DataPointList dataPointList){
        this.daysPrior = daysPrior;
        this.percent = percent;
        this.percent = percent.divide(new BigDecimal(100.0), RoundingMode.HALF_UP);
        this.action = action;
        this.dataPointList = dataPointList;
    }

    public boolean evaluate(Date today) throws NoValidDateFoundException{
        this.toDate = today;
        int indexOfToDate = this.dataPointList.getIndexOfData(today);
        if (indexOfToDate == -1)
            throw new NoValidDateFoundException();
        int indexOfFromDate = indexOfToDate - daysPrior;
        if(indexOfFromDate <0)
            throw new NoValidDateFoundException();
        this.fromDate = this.dataPointList.get(indexOfFromDate).getDate();
        List<DataPoint> tmpList = new ArrayList<DataPoint>();
        for(DataPoint point : dataPointList.getDataPoints()){
            Date pointDate = point.getDate();
            if(pointDate.compareTo(fromDate) >= 0 && pointDate.compareTo(toDate) <= 0){
                tmpList.add(point);
            }
        }
        if(tmpList.isEmpty()) throw new NoValidDateFoundException();
        int length = tmpList.size();
        DataPoint first = tmpList.get(0), last = tmpList.get(length-1);
        BigDecimal delta = last.getPrice().divide(first.getPrice(), RoundingMode.HALF_UP);
        boolean result = false;

        switch (action){
            case INCREASE:
                result = delta.compareTo(percent) >= 0;
                break;
            case DECREASE:
                result = delta.compareTo(percent) <= 0;
                break;
        }
        return result;
    }

}
