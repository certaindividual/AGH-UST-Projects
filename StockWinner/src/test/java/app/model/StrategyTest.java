package app.model;

import app.exceptions.NoValidDateFoundException;
import org.junit.Test;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.List;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class StrategyTest {

    @Test
    public void testEvaluateWithNoValidDates(){
        //given
        Date fromDate = new GregorianCalendar(2018,12,10).getTime();
        Date toDate = new GregorianCalendar(2018,12,12).getTime();
        BigDecimal percent = new BigDecimal(100.0);
        StrategyEnums.Change action = StrategyEnums.Change.INCREASE;
        StrategyEnums.Conditions condition = StrategyEnums.Conditions.NONE;

        DataPointList dataPointList = mock(DataPointList.class);
        List<DataPoint> dataPoints = new ArrayList<DataPoint>();
        Date pointDate = new GregorianCalendar(2018,12,13).getTime();
        dataPoints.add(new DataPoint(pointDate,percent));

        //when
        Strategy strategy = new Strategy(2,percent,action,dataPointList);
        when(dataPointList.getDataPoints()).thenReturn(dataPoints);

        //then
        try {
            strategy.evaluate(toDate);
        }
        catch (Exception ex){
            assertTrue(ex instanceof NoValidDateFoundException);
        }
    }

    @Test
    public void testEvaluateWithValidBuy(){
        //given
        Date fromDate = new GregorianCalendar(2018,12,8).getTime();
        Date toDate = new GregorianCalendar(2018,12,12).getTime();
        BigDecimal percent = new BigDecimal(200.0);
        StrategyEnums.Change action = StrategyEnums.Change.INCREASE;
        StrategyEnums.Conditions condition = StrategyEnums.Conditions.NONE;

        DataPointList dataPointList = mock(DataPointList.class);
        List<DataPoint> dataPoints = new ArrayList<DataPoint>();
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,8).getTime(),
                new BigDecimal(20.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,9).getTime(),
                new BigDecimal(30.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,10).getTime(),
                new BigDecimal(40.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,11).getTime(),
                new BigDecimal(50.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,12).getTime(),
                new BigDecimal(60.0)));

        //when
        Strategy strategy = new Strategy(4,percent,action,dataPointList);
        when(dataPointList.getDataPoints()).thenReturn(dataPoints);

        //then
        try {
            assertTrue(strategy.evaluate(toDate));
        }
        catch (Exception ex){ }

    }

    @Test
    public void testEvaluateWithInvalidBuy(){
        //given
        Date fromDate = new GregorianCalendar(2018,12,8).getTime();
        Date toDate = new GregorianCalendar(2018,12,12).getTime();
        BigDecimal percent = new BigDecimal(200.0);
        StrategyEnums.Change action = StrategyEnums.Change.INCREASE;
        StrategyEnums.Conditions condition = StrategyEnums.Conditions.NONE;

        DataPointList dataPointList = mock(DataPointList.class);
        List<DataPoint> dataPoints = new ArrayList<DataPoint>();
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,8).getTime(),
                new BigDecimal(20.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,9).getTime(),
                new BigDecimal(21.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,10).getTime(),
                new BigDecimal(22.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,11).getTime(),
                new BigDecimal(23.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,12).getTime(),
                new BigDecimal(30.0)));

        //when
        Strategy strategy = new Strategy(4,percent,action,dataPointList);
        when(dataPointList.getDataPoints()).thenReturn(dataPoints);

        //then
        try {
            assertFalse(strategy.evaluate(toDate));
        }
        catch (Exception ex){ }

    }

    @Test
    public void testEvaluateWithValidSell(){
        //given
        Date fromDate = new GregorianCalendar(2018,12,8).getTime();
        Date toDate = new GregorianCalendar(2018,12,12).getTime();
        BigDecimal percent = new BigDecimal(50.0);
        StrategyEnums.Change action = StrategyEnums.Change.DECREASE;
        StrategyEnums.Conditions condition = StrategyEnums.Conditions.NONE;

        DataPointList dataPointList = mock(DataPointList.class);
        List<DataPoint> dataPoints = new ArrayList<DataPoint>();
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,8).getTime(),
                new BigDecimal(60.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,9).getTime(),
                new BigDecimal(50.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,10).getTime(),
                new BigDecimal(40.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,11).getTime(),
                new BigDecimal(30.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,12).getTime(),
                new BigDecimal(20.0)));

        //when
        Strategy strategy = new Strategy(4,percent,action,dataPointList);
        when(dataPointList.getDataPoints()).thenReturn(dataPoints);

        //then
        try {
            assertTrue(strategy.evaluate(toDate));
        }
        catch (Exception ex){ }

    }

    @Test
    public void testEvaluateWithInvalidSell(){
        //given
        Date fromDate = new GregorianCalendar(2018,12,8).getTime();
        Date toDate = new GregorianCalendar(2018,12,12).getTime();
        BigDecimal percent = new BigDecimal(75.0);
        StrategyEnums.Change action = StrategyEnums.Change.DECREASE;
        StrategyEnums.Conditions condition = StrategyEnums.Conditions.NONE;

        DataPointList dataPointList = mock(DataPointList.class);
        List<DataPoint> dataPoints = new ArrayList<DataPoint>();
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,8).getTime(),
                new BigDecimal(20.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,9).getTime(),
                new BigDecimal(30.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,10).getTime(),
                new BigDecimal(25.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,11).getTime(),
                new BigDecimal(35.0)));
        dataPoints.add(new DataPoint(new GregorianCalendar(2018,12,12).getTime(),
                new BigDecimal(40.0)));

        //when
        Strategy strategy = new Strategy(4,percent,action,dataPointList);
        when(dataPointList.getDataPoints()).thenReturn(dataPoints);

        //then
        try {
            assertFalse(strategy.evaluate(toDate));
        }
        catch (Exception ex){ }

    }

}
