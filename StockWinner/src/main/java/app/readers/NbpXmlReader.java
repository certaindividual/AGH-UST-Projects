package app.readers;

import app.exceptions.InvalidContentException;
import app.model.DataPoint;
import app.model.DataPointList;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import org.w3c.dom.Element;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.File;
import java.io.IOException;
import java.math.BigDecimal;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;



public class NbpXmlReader extends FormatReader{

    public DataPointList getDataPointList(String path) throws InvalidContentException {
        List<DataPoint> pointsList = new ArrayList<DataPoint>();
        try {
            File inputFile = new File(path);
            DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
            Document doc = dBuilder.parse(inputFile);
            doc.getDocumentElement().normalize();       //recomended

            String source;
            Node root = doc.getDocumentElement();
            if(root.getNodeName().equals("ExchangeRatesSeries")) source = "Rate";
            else if(root.getNodeName().equals("ArrayOfCenaZlota")) source = "CenaZlota";
            else throw new InvalidContentException();

            NodeList nList = doc.getElementsByTagName(source); //Rates
            SimpleDateFormat ft = new SimpleDateFormat ("yyyy-MM-dd");

            for (int temp = 0; temp < nList.getLength(); temp++) {
                Node nNode = nList.item(temp);

                if (nNode.getNodeType() == Node.ELEMENT_NODE) {
                    Element eElement = (Element) nNode;
                    DataPoint dataPoint = new DataPoint();
                    if(eElement.getElementsByTagName("EffectiveDate").item(0) != null){
                        dataPoint.setDate(ft.parse(eElement.getElementsByTagName("EffectiveDate").item(0).getTextContent()));
                    }
                    else if(eElement.getElementsByTagName("Data").item(0) != null){
                        dataPoint.setDate(ft.parse(eElement.getElementsByTagName("Data").item(0).getTextContent()));
                    }

                    if(eElement.getElementsByTagName("Mid").item(0) != null) {
                        dataPoint.setPrice(new BigDecimal(eElement.getElementsByTagName("Mid").item(0).getTextContent()));
                    }
                    else if(eElement.getElementsByTagName("Cena").item(0) != null) {
                        dataPoint.setPrice(new BigDecimal(eElement.getElementsByTagName("Cena").item(0).getTextContent()));
                    }
                    pointsList.add(dataPoint);
                }
            }

        } catch (ParserConfigurationException e1) {

        } catch (IOException e1) {

        } catch (SAXException e1) {

        } catch (ParseException e) {
            throw new InvalidContentException();
        }
        return new DataPointList(pointsList);
    }
}

