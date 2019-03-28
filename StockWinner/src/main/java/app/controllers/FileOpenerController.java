package app.controllers;

import app.readers.WebSites;
import javafx.collections.FXCollections;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.stage.FileChooser;
import app.model.DataPointList;
import java.io.File;

public class FileOpenerController {

    private DataPointList dataPointList;

    private AppController appController;


    public void initialize(AppController appController) {
        this.appController = appController;
        this.websitesComboBox.setItems(FXCollections.observableArrayList(WebSites.SupportedWebSites.values()));
    }

    @FXML
    private Button chooseFileButton;

    @FXML
    private Button openFileButton;

    @FXML
    private TextField fileLocationField;

    @FXML
    private ComboBox<WebSites.SupportedWebSites> websitesComboBox;

    @FXML
    private void handleChoosingAction(ActionEvent event){

        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Resource File");
        fileChooser.setInitialDirectory(
                new File(System.getProperty("user.home"))
        );
        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("CSV File","*.csv"),
                new FileChooser.ExtensionFilter("JSON File", "*.json"),
                new FileChooser.ExtensionFilter("XML File", "*.xml")
        );
        File file = fileChooser.showOpenDialog(appController.getPrimaryStage());
        if(file != null){
            fileLocationField.setText(file.getAbsolutePath());
        }
    }

    @FXML
    private void handleOpenAction(ActionEvent event){
        if(fileLocationField.getText().equals("") || websitesComboBox.getSelectionModel().isEmpty()){
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("Warning");
            alert.setHeaderText("File or website was not selected");
            alert.setContentText("Choose a file and source website!");
            alert.showAndWait();
            return;
        }
        appController.setFileLocation(fileLocationField.getText());
        appController.setChosenWebsite(websitesComboBox.getValue());
        appController.initGraphScene("views/GraphView.fxml");
    }

}
