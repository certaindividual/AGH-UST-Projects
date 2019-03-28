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

public class NbpXmlReaderTest {

    SimpleDateFormat ft = new SimpleDateFormat("yyyy-MM-dd");

    File tempFileGold;      //http://api.nbp.pl/api/exchangerates/rates/a/gbp/2012-01-01/2012-01-31/?format=json
    {
        try {
            tempFileGold = File.createTempFile("json", ".json");
            BufferedWriter bw = new BufferedWriter(new FileWriter(tempFileGold));
            bw.write("<ArrayOfCenaZlota xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-02</Data>\n" +
                    "<Cena>165.83</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-03</Data>\n" +
                    "<Cena>166.97</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-04</Data>\n" +
                    "<Cena>167.43</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-07</Data>\n" +
                    "<Cena>167.98</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-08</Data>\n" +
                    "<Cena>167.26</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-09</Data>\n" +
                    "<Cena>167.48</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-10</Data>\n" +
                    "<Cena>167.98</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-11</Data>\n" +
                    "<Cena>167.59</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-14</Data>\n" +
                    "<Cena>164.61</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-15</Data>\n" +
                    "<Cena>165.18</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-16</Data>\n" +
                    "<Cena>166.14</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-17</Data>\n" +
                    "<Cena>167.58</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-18</Data>\n" +
                    "<Cena>166.14</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-21</Data>\n" +
                    "<Cena>167.89</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-22</Data>\n" +
                    "<Cena>170.11</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-23</Data>\n" +
                    "<Cena>170.34</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-24</Data>\n" +
                    "<Cena>169.51</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-25</Data>\n" +
                    "<Cena>169.23</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-28</Data>\n" +
                    "<Cena>166.44</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-29</Data>\n" +
                    "<Cena>165.50</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-30</Data>\n" +
                    "<Cena>167.01</Cena>\n" +
                    "</CenaZlota>\n" +
                    "<CenaZlota>\n" +
                    "<Data>2013-01-31</Data>\n" +
                    "<Cena>166.85</Cena>\n" +
                    "</CenaZlota>\n" +
                    "</ArrayOfCenaZlota>");
            bw.close();
            tempFileGold.deleteOnExit();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    NbpXmlReader readerGold = new NbpXmlReader();

    File tempFileGBP;      //http://api.nbp.pl/api/exchangerates/rates/a/gbp/2012-01-01/2012-01-31/?format=json
    {
        try {
            tempFileGBP = File.createTempFile("json", ".json");
            BufferedWriter bw = new BufferedWriter(new FileWriter(tempFileGBP));
            bw.write("<ExchangeRatesSeries xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n" +
                    "<Table>A</Table>\n" +
                    "<Currency>funt szterling</Currency>\n" +
                    "<Code>GBP</Code>\n" +
                    "<Rates>\n" +
                    "<Rate>\n" +
                    "<No>1/A/NBP/2012</No>\n" +
                    "<EffectiveDate>2012-01-02</EffectiveDate>\n" +
                    "<Mid>5.3480</Mid>\n" +
                    "</Rate>\n" +
                    "<Rate>\n" +
                    "<No>2/A/NBP/2012</No>\n" +
                    "<EffectiveDate>2012-01-03</EffectiveDate>\n" +
                    "<Mid>5.3394</Mid>\n" +
                    "</Rate>\n" +
                    "<Rate>\n" +
                    "<No>3/A/NBP/2012</No>\n" +
                    "<EffectiveDate>2012-01-04</EffectiveDate>\n" +
                    "<Mid>5.3712</Mid>\n" +
                    "</Rate>\n" +
                    "<Rate>\n" +
                    "<No>4/A/NBP/2012</No>\n" +
                    "<EffectiveDate>2012-01-05</EffectiveDate>\n" +
                    "<Mid>5.4616</Mid>\n" +
                    "</Rate>\n" +
                    "<Rate>\n" +
                    "<No>5/A/NBP/2012</No>\n" +
                    "<EffectiveDate>2012-01-09</EffectiveDate>\n" +
                    "<Mid>5.4269</Mid>\n" +
                    "</Rate>\n" +
                    "</Rates>\n" +
                    "</ExchangeRatesSeries>");
            bw.close();
            tempFileGBP.deleteOnExit();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    NbpXmlReader readerGBP = new NbpXmlReader();


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
        assertEquals("2013-01-02", ft.format(readerGold.getDataPointList(tempFileGold.getPath()).get(0).getDate()));
    }
    @Test
    public void DateFromFile3() throws InvalidContentException {
        assertEquals("2013-01-03", ft.format(readerGold.getDataPointList(tempFileGold.getPath()).get(1).getDate()));
    }
    @Test
    public void PriceFromFile2() throws InvalidContentException {
        assertEquals(BigDecimal.valueOf(166.97), readerGold.getDataPointList(tempFileGold.getPath()).get(1).getPrice());
    }

}
