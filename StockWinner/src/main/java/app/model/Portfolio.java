package app.model;

import java.math.BigDecimal;

public class Portfolio {

    private BigDecimal funds;

    private BigDecimal units;

    private BigDecimal currPriceOfUnits;

    public Portfolio(BigDecimal funds) {
        this.funds = funds;
        this.units = new BigDecimal(0);
        this.currPriceOfUnits = new BigDecimal(0);
    }
    public BigDecimal getCurrentPortfolioValue() {

       return funds.add(units.multiply(currPriceOfUnits));

    }

    public void updateValue(BigDecimal currPrice) {

        this.currPriceOfUnits = currPrice;
    }

    public void buy(BigDecimal percentOfFunds) {

        BigDecimal expense = funds.multiply(percentOfFunds);
        BigDecimal price = currPriceOfUnits;
        if (expense.compareTo(price) <0)
            return;
        if(funds.subtract(expense).compareTo(new BigDecimal(0)) <=0)
            return;
        BigDecimal unitsToBuy = expense.divide(price, BigDecimal.ROUND_DOWN);
        units = units.add(unitsToBuy);
        funds = funds.subtract(expense);
    }
    public void sell(BigDecimal percentOfUnitsValue) {
        BigDecimal currentUnitsValue = units.multiply(currPriceOfUnits);
        BigDecimal sellValue = currentUnitsValue.multiply(percentOfUnitsValue);
        if (sellValue.compareTo(currPriceOfUnits) <0)
            return;
        BigDecimal unitsToSell = sellValue.divide(currPriceOfUnits, BigDecimal.ROUND_DOWN);
        if(unitsToSell.compareTo(units) >0)
            return;
        units = units.subtract(unitsToSell);
        funds = funds.add(sellValue);
    }

}
