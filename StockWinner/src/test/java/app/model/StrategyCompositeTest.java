package app.model;

import app.exceptions.InvalidConditionException;
import org.junit.Test;


import java.util.Date;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class StrategyCompositeTest {

    @Test
    public void testEvaluateWithInvalidCondition(){
        //given
        Strategy firstStrategy = mock(Strategy.class);
        Strategy secondStrategy = mock(Strategy.class);
        Date date = mock(Date.class);
        StrategyComposite strategyComposite = new StrategyComposite(firstStrategy);
        strategyComposite.addStrategy(secondStrategy);
        strategyComposite.setCondition(StrategyEnums.Conditions.NONE);

        try {
            //when
            when(firstStrategy.evaluate(date)).thenReturn(true);
            when(secondStrategy.evaluate(date)).thenReturn(true);

            //then
            strategyComposite.evaluate(date);
        }
        catch (Exception ex){
            assertTrue(ex instanceof InvalidConditionException);
        }
    }

    @Test
    public void testEvaluateWithSingleStrategy(){
        //given
        Strategy firstStrategy = mock(Strategy.class);
        StrategyComposite strategyComposite = new StrategyComposite(firstStrategy);
        Date date = mock(Date.class);

        try {
            //when
            when(firstStrategy.evaluate(date)).thenReturn(true);

            //then
            assertTrue(strategyComposite.evaluate(date));
        }
        catch (Exception ex){ }
    }

    @Test
    public void testEvaluateWithMultipleAndTrueStrategy(){
        //given
        Strategy firstStrategy = mock(Strategy.class);
        Strategy secondStrategy = mock(Strategy.class);
        Strategy thirdStrategy = mock(Strategy.class);
        StrategyComposite strategyComposite = new StrategyComposite(firstStrategy);
        strategyComposite.addStrategy(secondStrategy);
        strategyComposite.addStrategy(thirdStrategy);
        strategyComposite.setCondition(StrategyEnums.Conditions.AND);
        Date date = mock(Date.class);

        try {
            //when
            when(firstStrategy.evaluate(date)).thenReturn(true);
            when(secondStrategy.evaluate(date)).thenReturn(true);
            when(thirdStrategy.evaluate(date)).thenReturn(true);

            //then
            assertTrue(strategyComposite.evaluate(date));
        }
        catch (Exception ex){ }
    }

    @Test
    public void testEvaluateWithMultipleAndFalseStrategy(){
        //given
        Strategy firstStrategy = mock(Strategy.class);
        Strategy secondStrategy = mock(Strategy.class);
        Strategy thirdStrategy = mock(Strategy.class);
        StrategyComposite strategyComposite = new StrategyComposite(firstStrategy);
        strategyComposite.addStrategy(secondStrategy);
        strategyComposite.addStrategy(thirdStrategy);
        strategyComposite.setCondition(StrategyEnums.Conditions.AND);
        Date date = mock(Date.class);

        try {
            //when
            when(firstStrategy.evaluate(date)).thenReturn(true);
            when(secondStrategy.evaluate(date)).thenReturn(false);
            when(thirdStrategy.evaluate(date)).thenReturn(true);

            //then
            assertFalse(strategyComposite.evaluate(date));
        }
        catch (Exception ex){ }
    }

    @Test
    public void testEvaluateWithMultipleOrTrueStrategy(){
        //given
        Strategy firstStrategy = mock(Strategy.class);
        Strategy secondStrategy = mock(Strategy.class);
        Strategy thirdStrategy = mock(Strategy.class);
        StrategyComposite strategyComposite = new StrategyComposite(firstStrategy);
        strategyComposite.addStrategy(secondStrategy);
        strategyComposite.addStrategy(thirdStrategy);
        strategyComposite.setCondition(StrategyEnums.Conditions.OR);
        Date date = mock(Date.class);

        try {
            //when
            when(firstStrategy.evaluate(date)).thenReturn(true);
            when(secondStrategy.evaluate(date)).thenReturn(false);
            when(thirdStrategy.evaluate(date)).thenReturn(true);

            //then
            assertTrue(strategyComposite.evaluate(date));
        }
        catch (Exception ex){ }
    }

    @Test
    public void testEvaluateWithMultipleOrFalseStrategy(){
        //given
        Strategy firstStrategy = mock(Strategy.class);
        Strategy secondStrategy = mock(Strategy.class);
        Strategy thirdStrategy = mock(Strategy.class);
        StrategyComposite strategyComposite = new StrategyComposite(firstStrategy);
        strategyComposite.addStrategy(secondStrategy);
        strategyComposite.addStrategy(thirdStrategy);
        strategyComposite.setCondition(StrategyEnums.Conditions.OR);
        Date date = mock(Date.class);

        try {
            //when
            when(firstStrategy.evaluate(date)).thenReturn(false);
            when(secondStrategy.evaluate(date)).thenReturn(false);
            when(thirdStrategy.evaluate(date)).thenReturn(false);

            //then
            assertFalse(strategyComposite.evaluate(date));
        }
        catch (Exception ex){ }
    }

}
