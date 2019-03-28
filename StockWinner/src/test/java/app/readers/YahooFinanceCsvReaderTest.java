package app.readers;
import app.exceptions.InvalidContentException;
import org.junit.Test;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.math.BigDecimal;
import java.text.SimpleDateFormat;

import static org.junit.Assert.*;

public class YahooFinanceCsvReaderTest {

    File tempFileCSV;      //http://api.nbp.pl/api/exchangerates/rates/a/gbp/2012-01-01/2012-01-31/?format=json
    {
        try {
            tempFileCSV = File.createTempFile("json", ".json");
            BufferedWriter bw = new BufferedWriter(new FileWriter(tempFileCSV));
            bw.write("Date,Open,High,Low,Close,Adj Close,Volume\n" +
                    "2018-10-30,18.910000,19.120001,18.540001,18.870001,18.786798,254400\n" +
                    "2018-10-31,19.110001,19.290001,18.889999,19.059999,18.975960,175300\n" +
                    "2018-11-01,18.780001,18.780001,14.570000,14.970000,14.903995,822500\n" +
                    "2018-11-02,15.020000,15.420000,14.900000,15.340000,15.272363,305000\n" +
                    "2018-11-05,15.300000,16.629999,15.300000,15.730000,15.660643,263000");
            bw.close();
            tempFileCSV.deleteOnExit();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    SimpleDateFormat ft = new SimpleDateFormat("yyyy-MM-dd");
    YahooFinanceCsvReader readerCSV = new YahooFinanceCsvReader();

    @Test
    public void DateFromFile1() throws InvalidContentException {
        assertEquals("2018-10-30", ft.format(readerCSV.getDataPointList(tempFileCSV.getPath()).get(0).getDate()));
    }

    @Test
    public void DateFromFile2() throws InvalidContentException {
        assertEquals("2018-11-02", ft.format(readerCSV.getDataPointList(tempFileCSV.getPath()).get(3).getDate()));
    }

    @Test
    public void PriceFromFile1() throws InvalidContentException {
        assertEquals(BigDecimal.valueOf(19.110001), readerCSV.getDataPointList(tempFileCSV.getPath()).get(1).getPrice());
    }
    @Test
    public void ListSize() throws InvalidContentException {
        assertEquals(5, readerCSV.getDataPointList(tempFileCSV.getPath()).size());
    }

}
