package app.controllers;

import app.exceptions.InvalidContentException;
import app.exceptions.InvalidConditionException;
import app.exceptions.NoValidDateFoundException;
import app.model.*;
import app.readers.*;
import app.support.ZoomManager;
import javafx.collections.FXCollections;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.chart.CategoryAxis;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Alert;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.layout.Pane;

import java.io.IOException;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class GraphDialogController {

    private AppController appController;

    @FXML
    private CategoryAxis xAxis;

    @FXML
    private NumberAxis yAxis;

    @FXML
    private LineChart<String, Number> lineChart;

    @FXML
    private TextField daysPrior;

    @FXML
    private ComboBox<StrategyEnums.Change> change;

    @FXML
    private ComboBox<StrategyEnums.Conditions> condition;

    @FXML
    private TextField percent;

    @FXML
    private ListView<String> condList;

    @FXML
    private ComboBox<StrategyEnums.Decision> decision;

    @FXML
    private TextField percentOfFundsOrPortfolio;

    @FXML
    private TextField funds;


    private DataPointList pointList;

    private DataPointList resultsPointsList;

    private List<StrategyComposite> strategyCompositeList;

    private boolean secondStrategy = false;

    Pane chartParent;

    public void initialize(AppController controller, String fileLocation, WebSites.SupportedWebSites chosenWebsite, Pane chartParent) throws IOException {

        this.appController = controller;
        this.chartParent = chartParent;
        this.change.setItems(FXCollections.observableArrayList(StrategyEnums.Change.values()));
        this.condition.getItems().addAll(
            StrategyEnums.Conditions.AND,
                StrategyEnums.Conditions.OR
        );
        this.decision.getItems().addAll(
                StrategyEnums.Decision.BUY,
                StrategyEnums.Decision.SELL
        );
        this.strategyCompositeList = new ArrayList<StrategyComposite>();

        xAxis.setLabel("Date");
        yAxis.setLabel("Price");

        XYChart.Series series = new XYChart.Series();
        series.setName("Price fluctuations");

        ReaderFactory readerFactory = new ReaderFactory();
        FormatReader reader = readerFactory.concreteReader(fileLocation, chosenWebsite);
        try {
            pointList = reader.getDataPointList(fileLocation);

        } catch (InvalidContentException e) {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("Warning");
            alert.setHeaderText("Invalid content of file");
            alert.setContentText("Choose another file");
            alert.showAndWait();
        }
        
        for(DataPoint dp: pointList.getDataPoints()) {
            series.getData().add(new XYChart.Data(dp.getDate().toString(), dp.getPrice()));
        }
        new ZoomManager(chartParent, lineChart,series);
    }


    @FXML
    private void handleGoingBack(ActionEvent event){

        appController.goBackToFirstWindow();
    }

    @FXML
    private void handleReset(ActionEvent event){

        appController.initGraphScene("views/GraphView.fxml");
    }
    @FXML
    private void handleAddCondButton(ActionEvent event){

        if((secondStrategy && condition.getSelectionModel().isEmpty()) ||
            daysPrior.getText().isEmpty() ||
            percent.getText().isEmpty() ||
            change.getSelectionModel().isEmpty()) {

            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Value not set");
            alert.setContentText("Please set all needed values");
            alert.showAndWait();
            return;
        }
        Pattern pattern = Pattern.compile("^[0-9]+$");
        Matcher matcher1 = pattern.matcher(daysPrior.getText());
        Matcher matcher2 = pattern.matcher(percent.getText());

        if(!(matcher1.find()  && matcher2.find() && daysPrior.getText().compareTo("0") > 0)) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Incorrect days prior or percent value");
            alert.setContentText("Field value should be integer >0");
            alert.showAndWait();
            return;
        }
        Double percentValue = new Double(percent.getText());
        if(percentValue <0) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Incorrect percent");
            alert.setContentText("Percent change should not be < 0");
            alert.showAndWait();
            return;
        }
        if(change.getValue().equals(StrategyEnums.Change.INCREASE))
            percentValue += 100.0;
        else {
            percentValue = 100.0 - percentValue;
        }
        BigDecimal val = new BigDecimal(percentValue);
        int daysPriorInteger = Integer.parseInt(daysPrior.getText());

        if(condition.getSelectionModel().isEmpty()){

            condition.setDisable(false);
            IStrategyComponent strategy = new Strategy(daysPriorInteger,
                    val, change.getValue(), pointList);
            this.strategyCompositeList.add(new StrategyComposite(strategy));
            decision.setDisable(false);
            percentOfFundsOrPortfolio.setDisable(false);
            secondStrategy = true;

        }
        else {
            condition.setDisable(true);
            IStrategyComponent strategy = new Strategy(daysPriorInteger,
                    val, change.getValue(), pointList);
            this.strategyCompositeList.get(this.strategyCompositeList.size() - 1).addStrategy(strategy);
            this.strategyCompositeList.get(this.strategyCompositeList.size() - 1).setCondition(condition.getValue());
        }


        condList.getItems().add(" Days prior " + daysPriorInteger
                + " percent change " + percent.getText()
                + " change " + change.getValue().toString());


    }

    @FXML
    private void handleDoneButton(ActionEvent event){

        xAxis.setLabel("Date");
        yAxis.setLabel("Value");

        XYChart.Series series = new XYChart.Series();
        series.setName("Value of portfolio over time");

        Pattern pattern = Pattern.compile("^[0-9]+$");
        Matcher matcher = pattern.matcher(funds.getText());

        if(!(matcher.find()  && funds.getText().compareTo("0") > 0) || funds.getText().isEmpty()) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Incorrect start funds");
            alert.setContentText("Field value should be integer >0");
            alert.showAndWait();
            return;
        }

        this.resultsPointsList = new DataPointList(new ArrayList<DataPoint>());
        Portfolio portfolio = new Portfolio(new BigDecimal(funds.getText()));

        Boolean sellOrBuyAction = false;

        for (DataPoint dp : pointList.getDataPoints()) {

            portfolio.updateValue(dp.getPrice());
            for (StrategyComposite sc : strategyCompositeList) {
                try {
                    sellOrBuyAction = sc.evaluate(dp.getDate());
                }
                catch (NoValidDateFoundException | InvalidConditionException ex) {
                    //pass
                }
                if(sellOrBuyAction) {
                    switch (sc.getDecision()){
                        case BUY:

                            portfolio.buy(sc.getPercentOfFundsOrPortfolio());
                            break;
                        case SELL:
                            portfolio.sell(sc.getPercentOfFundsOrPortfolio());
                            break;
                    }
                }
            }
            resultsPointsList.addDataPointToList(new DataPoint(dp.getDate(),portfolio.getCurrentPortfolioValue()));
        }
        appController.setResultsList(this.resultsPointsList);
        appController.initResultsScene("views/ResultsView.fxml");
    }

    @FXML
    private void handleAddDecisionButton(ActionEvent event) {

        if(decision.getSelectionModel().isEmpty() || percentOfFundsOrPortfolio.getText().isEmpty() ) {

            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Decision not set");
            alert.setContentText("Please set all needed values");
            alert.showAndWait();
            return;
        }
        Pattern pattern = Pattern.compile("^[0-9]+$");
        Matcher matcher = pattern.matcher(percentOfFundsOrPortfolio.getText());

        if(!(matcher.find()  && percentOfFundsOrPortfolio.getText().compareTo("0") > 0)) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("Error");
            alert.setHeaderText("Incorrect percent");
            alert.setContentText("Field value should be integer >0");
            alert.showAndWait();
            return;
        }
        this.strategyCompositeList.get(this.strategyCompositeList.size() - 1).setDecision(decision.getValue());
        BigDecimal percentOfFP = new BigDecimal(percentOfFundsOrPortfolio.getText());
        percentOfFP = percentOfFP.divide(new BigDecimal(100), 10, RoundingMode.HALF_UP);
        this.strategyCompositeList.get(this.strategyCompositeList.size() - 1).setPercentOfFundsOrPortfolio(percentOfFP);

        condList.getItems().add("                THEN " + decision.getValue().toString()
                + " " + percentOfFundsOrPortfolio.getText()
                + " percent ");

        secondStrategy = false;
        daysPrior.clear();
        change.getSelectionModel().clearSelection();
        change.setDisable(false);
        condition.getSelectionModel().clearSelection();
        condition.setDisable(true);
        percent.clear();
        decision.getSelectionModel().clearSelection();
        decision.setDisable(true);
        percentOfFundsOrPortfolio.clear();
        percentOfFundsOrPortfolio.setDisable(true);

    }

}
