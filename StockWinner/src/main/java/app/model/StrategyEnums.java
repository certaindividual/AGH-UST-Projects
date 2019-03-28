package app.model;

public class StrategyEnums {
    public enum Conditions{
        AND,
        OR,
        NONE, //condition for first or single strategy
    }
    public enum Change {
        DECREASE,
        INCREASE,
    }
    public enum Decision {
        BUY,
        SELL,
    }
}
