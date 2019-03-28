package app.model;

import org.junit.Test;

import java.math.BigDecimal;

import static org.junit.Assert.*;

public class PortfolioTest {
    @Test
    public void getCurrentPriceOfUnitsTest(){
        Portfolio portfolio = new Portfolio(BigDecimal.valueOf(100));
        assertEquals(BigDecimal.valueOf(100), portfolio.getCurrentPortfolioValue());
    }

    @Test
    public void buyTest1(){
        Portfolio portfolio = new Portfolio(BigDecimal.valueOf(100));
        portfolio.buy(BigDecimal.valueOf(10));
        assertEquals(BigDecimal.valueOf(100), portfolio.getCurrentPortfolioValue());
    }

    @Test
    public void buyTest2(){
        Portfolio portfolio = new Portfolio(BigDecimal.valueOf(100));
        portfolio.updateValue(BigDecimal.valueOf(100));
        portfolio.buy(BigDecimal.valueOf(10));
        assertEquals(BigDecimal.valueOf(100), portfolio.getCurrentPortfolioValue());
    }

    @Test
    public void sellTest() {
        Portfolio portfolio = new Portfolio(BigDecimal.valueOf(100));
        portfolio.updateValue(BigDecimal.valueOf(100));
        portfolio.sell(BigDecimal.valueOf(10));
        assertEquals(BigDecimal.valueOf(100), portfolio.getCurrentPortfolioValue());
    }

    @Test
    public void buySellTest() {
        Portfolio portfolio = new Portfolio(BigDecimal.valueOf(100));
        portfolio.updateValue(BigDecimal.valueOf(100));
        portfolio.sell(BigDecimal.valueOf(10));
        portfolio.buy(BigDecimal.valueOf(20));
        assertEquals(BigDecimal.valueOf(100), portfolio.getCurrentPortfolioValue());

    }

    @Test
    public void buySellTest2() {
        Portfolio portfolio = new Portfolio(BigDecimal.valueOf(100));
        portfolio.updateValue(BigDecimal.valueOf(100));
        portfolio.buy(BigDecimal.valueOf(25));
        portfolio.sell(BigDecimal.valueOf(10));
        assertEquals(BigDecimal.valueOf(100), portfolio.getCurrentPortfolioValue());
    }

    @Test
    public void buySellTest3() {
        Portfolio portfolio = new Portfolio(BigDecimal.valueOf(100));
        portfolio.updateValue(BigDecimal.valueOf(140));
        portfolio.buy(BigDecimal.valueOf(25));
        portfolio.updateValue(BigDecimal.valueOf(152));
        portfolio.sell(BigDecimal.valueOf(10));
        assertEquals(BigDecimal.valueOf(100), portfolio.getCurrentPortfolioValue());
    }
}
