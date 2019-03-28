package app.readers;

import org.junit.Test;

import java.io.IOException;

import static org.hamcrest.CoreMatchers.instanceOf;
import static org.junit.Assert.*;

public class ReaderFactoryTest {
    @Test
    public void CsvTest() throws IOException {
        ReaderFactory readerFactory = new ReaderFactory();
        FormatReader csvReader = readerFactory.concreteReader("file.csv", WebSites.SupportedWebSites.YAHOOFINANCE);
        assertThat(csvReader, instanceOf(YahooFinanceCsvReader.class));
    }
    @Test
    public void JsonTest() throws IOException {
        ReaderFactory readerFactory = new ReaderFactory();
        FormatReader reader = readerFactory.concreteReader("file.json", WebSites.SupportedWebSites.NBP);
        assertThat(reader, instanceOf(NbpJsonReader.class));
    }
    @Test
    public void XmlTest() throws IOException {
        ReaderFactory readerFactory = new ReaderFactory();
        FormatReader reader = readerFactory.concreteReader("file.xml", WebSites.SupportedWebSites.NBP);
        assertThat(reader, instanceOf(NbpXmlReader.class));
    }
    @Test(expected = IOException.class)
    public void wrongFormatTest() throws IOException {
        ReaderFactory readerFactory = new ReaderFactory();
        FormatReader reader = readerFactory.concreteReader("file.java", WebSites.SupportedWebSites.NBP);
        assertThat(reader, instanceOf(NbpXmlReader.class));
    }
}