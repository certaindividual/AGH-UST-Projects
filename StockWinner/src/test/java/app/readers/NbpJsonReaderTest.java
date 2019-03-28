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

public class NbpJsonReaderTest {

    File tempFileGBP;      //http://api.nbp.pl/api/exchangerates/rates/a/gbp/2012-01-01/2012-01-31/?format=json
    {
        try {
            tempFileGBP = File.createTempFile("json", ".json");
            BufferedWriter bw = new BufferedWriter(new FileWriter(tempFileGBP));
            bw.write("{\"table\":\"A\",\"currency\":\"funt szterling\",\"code\":\"GBP\",\"rates\":[{\"no\":\"1/A/NBP/2012\",\"effectiveDate\":\"2012-01-02\",\"mid\":5.3480},{\"no\":\"2/A/NBP/2012\",\"effectiveDate\":\"2012-01-03\",\"mid\":5.3394},{\"no\":\"3/A/NBP/2012\",\"effectiveDate\":\"2012-01-04\",\"mid\":5.3712},{\"no\":\"4/A/NBP/2012\",\"effectiveDate\":\"2012-01-05\",\"mid\":5.4616},{\"no\":\"5/A/NBP/2012\",\"effectiveDate\":\"2012-01-09\",\"mid\":5.4269}]}");
            bw.close();
            tempFileGBP.deleteOnExit();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    SimpleDateFormat ft = new SimpleDateFormat("yyyy-MM-dd");
    NbpJsonReader readerGBP = new NbpJsonReader();

    File tempFileGold;      //http://api.nbp.pl/api/exchangerates/rates/a/gbp/2012-01-01/2012-01-31/?format=json
    {
        try {
            tempFileGold = File.createTempFile("json", ".json");
            BufferedWriter bw = new BufferedWriter(new FileWriter(tempFileGold));
            bw.write("[{\"data\":\"2018-10-18\",\"cena\":146.71},{\"data\":\"2018-10-19\",\"cena\":146.67},{\"data\":\"2018-10-22\",\"cena\":148.45},{\"data\":\"2018-10-23\",\"cena\":146.48},{\"data\":\"2018-10-24\",\"cena\":148.97},{\"data\":\"2018-10-25\",\"cena\":149.13},{\"data\":\"2018-10-26\",\"cena\":149.56},{\"data\":\"2018-10-29\",\"cena\":150.35},{\"data\":\"2018-10-30\",\"cena\":150.09},{\"data\":\"2018-10-31\",\"cena\":149.96},{\"data\":\"2018-11-02\",\"cena\":151.21}]");
            bw.close();
            tempFileGold.deleteOnExit();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    NbpJsonReader readerGold = new NbpJsonReader();

    //testy dla walut
    @Test
    public void DateFromFile1() throws InvalidContentException {
        assertEquals("2012-01-02", ft.format(readerGBP.getDataPointList(tempFileGBP.getPath()).get(0).getDate()));
    }
    @Test
    public void PriceFromFile1() throws InvalidContentException {
        assertEquals(BigDecimal.valueOf(5.3394), readerGBP.getDataPointList(tempFileGBP.getPath()).get(1).getPrice());
    }
    @Test
    public void ListSize() throws InvalidContentException {
        assertEquals(5, readerGBP.getDataPointList(tempFileGBP.getPath()).size());
    }



    //testy dla zlota
    @Test
    public void DateFromFile2() throws InvalidContentException {
        assertEquals("2018-10-18", ft.format(readerGold.getDataPointList(tempFileGold.getPath()).get(0).getDate()));
    }
    @Test
    public void PriceFromFile2() throws InvalidContentException {
        assertEquals(BigDecimal.valueOf(146.67), readerGold.getDataPointList(tempFileGold.getPath()).get(1).getPrice());
    }

}
