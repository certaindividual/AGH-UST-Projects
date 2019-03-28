package app.readers;

import app.exceptions.InvalidContentException;
import app.model.DataPointList;


public abstract class FormatReader {

    public abstract DataPointList getDataPointList(String path) throws InvalidContentException;
}
