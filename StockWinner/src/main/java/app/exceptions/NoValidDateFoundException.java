package app.exceptions;

import java.util.concurrent.Executors;

public class NoValidDateFoundException extends Exception {

    public NoValidDateFoundException(){
        super("No valid date was found in given interval");
    }

}
