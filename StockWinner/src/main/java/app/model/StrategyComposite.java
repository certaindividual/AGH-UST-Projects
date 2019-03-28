package app.model;

import app.exceptions.InvalidConditionException;
import app.exceptions.NoValidDateFoundException;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Date;

public class StrategyComposite implements IStrategyComponent {

    private ArrayList<IStrategyComponent> strategies;
    private StrategyEnums.Conditions condition;
    private StrategyEnums.Decision decision;
    private BigDecimal percentOfFundsOrPortfolio;

    public StrategyComposite(IStrategyComponent strategyComponent){
        this.strategies = new ArrayList<>();
        this.strategies.add(strategyComponent);
    }

    public void addStrategy(IStrategyComponent strategy){
        strategies.add(strategy);
    }

    public void removeStrategy(IStrategyComponent strategy){
        strategies.remove(strategy);
    }

    public void setCondition(StrategyEnums.Conditions condition){
        this.condition = condition;
    }

    public StrategyEnums.Conditions getCondition() {
        return condition;
    }

    public void setDecision(StrategyEnums.Decision decision) {
        this.decision = decision;
    }
    public StrategyEnums.Decision getDecision() {
        return this.decision;
    }

    public void setPercentOfFundsOrPortfolio(BigDecimal percentOfFundsOrPortfolio) {
        this.percentOfFundsOrPortfolio = percentOfFundsOrPortfolio;
    }

    public BigDecimal getPercentOfFundsOrPortfolio() {
        return this.percentOfFundsOrPortfolio;
    }

    public boolean evaluate(Date today) throws NoValidDateFoundException, InvalidConditionException {
        boolean result = strategies.get(0).evaluate(today);
        for(int i = 1; i < strategies.size(); ++i){
            IStrategyComponent strategy = strategies.get(i);
            switch (this.condition){
                case AND:
                    result = result && strategy.evaluate(today);
                    break;
                case OR:
                    result = result || strategy.evaluate(today);
                    break;
                case NONE:
                    throw new InvalidConditionException();
            }
        }
        return result;
    }
}
