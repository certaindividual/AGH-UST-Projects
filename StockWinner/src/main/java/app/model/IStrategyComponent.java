package app.model;

import app.exceptions.InvalidConditionException;
import app.exceptions.NoValidDateFoundException;

import java.util.Date;

public interface IStrategyComponent {

    boolean evaluate(Date today) throws NoValidDateFoundException, InvalidConditionException;

}
