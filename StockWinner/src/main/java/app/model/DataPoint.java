package app.model;

import java.math.BigDecimal;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class DataPoint {
    private Date date;
    private BigDecimal price;

    public DataPoint(Date date, BigDecimal price){
        this.date = date;
        this.price = price;
    }

    public Date getDate() {
        return date;
    }

    public void setDate(Date date) {
        this.date = date;
    }

    public BigDecimal getPrice() {
        return price;
    }

    public void setPrice(BigDecimal price) {
        if(price.compareTo(BigDecimal.ZERO) < 0) throw new IllegalArgumentException("Price cannot be lower than zero");
        else this.price = price;
    }

    public DataPoint(){}

}
