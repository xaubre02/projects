/*     Semestralni projekt
---------------------------------
Aplikace: Pasians Klondike
Predmet: IJA
Rok: 2016/2017
Autori: Aubrecht Tomas (xaubre02)
        Bandik Matej   (xbandi01)
-------------------------------*/

package src.GUI;


import javafx.event.EventHandler;
import javafx.geometry.Bounds;
import javafx.scene.effect.DropShadow;
import src.Model.Card;
import src.Util.GameSet;
import src.Util.Help;
import src.Util.Saves;


import javafx.application.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.scene.control.*;
import javafx.scene.input.TransferMode;

import javafx.geometry.Insets;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.MouseEvent;

import javafx.scene.Scene;
import javafx.stage.Stage;

import java.util.List;
import java.util.Optional;
import java.util.Stack;
import java.io.*;

/**
 * Aplikacna trieda pre Solitaire.
 * Trieda vytara hlavne komponenty hry a spusťa aplikaciu.
 * Logika hry je vytvarana nad jej elementami az za behu hry.
 */
public class Solitaire extends Application {


    private int WIDTH = 1200;
    private int HEIGHT = 900;

    private Card holdingCard;

    private GameSet gameSet1;
    private GameSet gameSet2;
    private GameSet gameSet3;
    private GameSet gameSet4;

    private Pane pane1;
    private Pane pane2;
    private Pane pane3;
    private Pane pane4;
    private Saves saveG = new Saves();

    private boolean cardFromGamePack = false;
    private boolean cardFromTargetPack = false;
    private boolean cardFromWorkingPack = false;
    private boolean undo = false;
    private boolean createNewGame = false;
    private boolean LoadGame = false;
    private boolean resetGame = false;

    private int numberOfGame = 0;
    private int takeFromColumn;
    private int takeFromIndex;
    private int clickOnGridNumber;

    private ImageView holdingImage;
    private Image backCard = new Image(new File("lib/pic/back1.jpg").toURI().toString());
    private Image emptyCard = new Image(new File("lib/pic/emptyPack.jpg").toURI().toString());
    private Image backCard2 = new Image(new File("lib/pic/back.jpg").toURI().toString());

    private Stack<GameSet> gameStack = new Stack<>();
    private Stack<Integer> gridStack = new Stack<>();

    private Stack<Image> imageStack = new Stack<>();
    private Stack<String> helpImageStack = new Stack<>();

    private GameSet gameSet;

    private GridPane gameGrid = new GridPane();

    private void setImageStack(){
        for(int i = 1; i <= 13; i++){
            for (int j = 1; j <= 4; j++){
                if(j == 1 ){
                    helpImageStack.push(i+"C");
                    imageStack.push(new Image(new File("lib/pic/"+i+"C.jpg").toURI().toString()));
                }
                if(j == 2 ){
                    helpImageStack.push(i+"D");
                    imageStack.push(new Image(new File("lib/pic/"+i+"D.jpg").toURI().toString()));
                }
                if(j == 3 ){
                    helpImageStack.push(i+"H");
                    imageStack.push(new Image(new File("lib/pic/"+i+"H.jpg").toURI().toString()));
                }
                if(j == 4 ){
                    helpImageStack.push(i+"S");
                    imageStack.push(new Image(new File("lib/pic/"+i+"S.jpg").toURI().toString()));
                }
            }
        }
    }


    private Image getImage(String nameCard){
        return imageStack.get(helpImageStack.indexOf(nameCard));
    }

    /**
     * Spustenie aplikacie
     * @param args Aplikacia nema ziadne argumenty
     */
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage stage) throws Exception{
        setImageStack();
        gameGrid.setHgap(20);
        gameGrid.setVgap(20);
        gameGrid.setStyle("-fx-background-color: #E3E3E3;");

        HBox hBox = new HBox();
        hBox.setSpacing(10);
        hBox.setStyle("-fx-background-color: #313131; -fx-padding: 10;");

        Button newGame = new Button("+");

        newGame.setOnAction(e -> {
            gameSet = new GameSet();
            gameSet.createNewGame();
            Pane pane = new Pane();

            createNewGame(pane, gameSet, gameGrid);
        });

        Help helper = new Help();
        Button help = new Button("?");
        help.setOnAction(e -> {
            if(clickOnGridNumber != 0) {
                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setTitle("Next move");
                alert.setHeaderText(null);

                if      (clickOnGridNumber == 1) alert.setContentText(helper.getHelp(gameSet1));
                else if (clickOnGridNumber == 2) alert.setContentText(helper.getHelp(gameSet2));
                else if (clickOnGridNumber == 3) alert.setContentText(helper.getHelp(gameSet3));
                else if (clickOnGridNumber == 4) alert.setContentText(helper.getHelp(gameSet4));

                alert.showAndWait();
            } else {
                clickFirst();
            }
        });

        Button stepBack = new Button("←");
        stepBack.setOnAction(e -> {
            if(clickOnGridNumber != 0) {
                if (!gameStack.empty()) {
                    int indexGame = -1;
                    for (int i = gridStack.size() - 1; i >= 0; i--) {
                        if (gridStack.get(i) == clickOnGridNumber) {
                            indexGame = i;
                            gridStack.remove(i);
                            break;
                        }
                    }
                    if (indexGame != -1) {
                        undo = true;
                        restartGame(gameStack.remove(indexGame));
                    }
                }
            } else {
                clickFirst();
            }
        });

        Button save = new Button("Save");
        save.setOnAction(e -> {
            if(clickOnGridNumber != 0) {
                TextInputDialog dialog = new TextInputDialog("save1");
                dialog.setTitle("Save game");
                dialog.setHeaderText(null);
                dialog.setContentText("Please enter a file name:");

                Optional<String> result = dialog.showAndWait();
                if (result.isPresent()) {
                    try {
                        if (clickOnGridNumber == 1) saveG.saveGame(gameSet1, result.get());
                        else if (clickOnGridNumber == 2) saveG.saveGame(gameSet2, result.get());
                        else if (clickOnGridNumber == 3) saveG.saveGame(gameSet3, result.get());
                        else if (clickOnGridNumber == 4) saveG.saveGame(gameSet4, result.get());
                    } catch (IOException io) {
                        io.printStackTrace();
                    }
                }
            } else {
                clickFirst();
            }
        });

        Button load = new Button("Load");

        load.setOnAction(e -> {

            if (clickOnGridNumber == 0){
                clickOnGridNumber = 1;
            }
            List<String> choices = saveG.getSaves();

            ChoiceDialog<String> dialog = new ChoiceDialog<>("example", choices);
            dialog.setTitle("Load game");
            dialog.setHeaderText(null);
            dialog.setContentText("Choose a saved game:");

            Optional<String> result = dialog.showAndWait();
            if (result.isPresent())
            {
                try
                {
                    LoadGame = true;
                    restartGame(saveG.loadGame(result.get()));
                }
                catch(IOException | ClassNotFoundException io) { io.printStackTrace(); }
            }

        });

        Button restartGame = new Button("Restart");

        restartGame.setOnAction(e -> {
            resetGame = true;
            gameSet = new GameSet();
            gameSet.createNewGame();
            restartGame(gameSet);
        });


        Button exitGame = new Button("Exit");

        exitGame.setOnAction(e -> {
            Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
            alert.setTitle("Exit Solitaire");
            alert.setHeaderText("Exit Window");
            alert.setContentText("Do you really want to exit?");

            Optional<ButtonType> result = alert.showAndWait();
            if (result.get() == ButtonType.OK){
                stage.close();
            }
        });


        Button removeGame = new Button("Remove");

        removeGame.setOnAction(e -> {
            Pane pane = new Pane();
            pane.setStyle("-fx-background-color: #E3E3E3;");

            if (clickOnGridNumber == 1) {
                gameGrid.add(pane, 0, 1);
            } else if (clickOnGridNumber == 2) {
                gameGrid.add(pane, 1, 1);
            } else if (clickOnGridNumber == 3) {
                gameGrid.add(pane, 0, 2);
            } else if (clickOnGridNumber == 4) {
                gameGrid.add(pane, 1, 2);
            }
        });


        gameGrid.getChildren().addAll(newGame, help, stepBack, save, load, restartGame, removeGame,exitGame);

        GridPane.setMargin(help, new Insets(0, 0, 0, 40));
        GridPane.setMargin(stepBack, new Insets(0, 0, 0, 80));
        GridPane.setMargin(save, new Insets(0, 0, 0, 120));
        GridPane.setMargin(load, new Insets(0, 0, 0, 180));
        GridPane.setMargin(restartGame, new Insets(0, 0, 0, 240));
        GridPane.setMargin(removeGame, new Insets(0, 0, 0, 320));
        GridPane.setMargin(exitGame, new Insets(0, 0, 0, 400));
        gameSet = new GameSet();
        gameSet.createNewGame();
        Pane pane = new Pane();
        pane1 = pane;

        gameGrid.setOnDragOver(e -> {
            if(e.getY() < (HEIGHT/2)-40 && e.getX() < (WIDTH/2)-20){ clickOnGridNumber = 1; }
            else if(e.getY() < (HEIGHT/2)-40 && e.getX() > (WIDTH/2)-20){ clickOnGridNumber = 2; }
            else if(e.getY() > (HEIGHT/2)-40 && e.getX() < (WIDTH/2)-20){ clickOnGridNumber = 3; }
            else if(e.getY() > (HEIGHT/2)-40 && e.getX() > (WIDTH/2)-20){ clickOnGridNumber = 4; }
            undo = false;

        });
        gameGrid.setOnMouseClicked(e -> {
            if(e.getY() < (HEIGHT/2)-40 && e.getX() < (WIDTH/2)-20){ clickOnGridNumber = 1; }
            else if(e.getY() < (HEIGHT/2)-40 && e.getX() > (WIDTH/2)-20){ clickOnGridNumber = 2; }
            else if(e.getY() > (HEIGHT/2)-40 && e.getX() < (WIDTH/2)-20){ clickOnGridNumber = 3; }
            else if(e.getY() > (HEIGHT/2)-40 && e.getX() > (WIDTH/2)-20){ clickOnGridNumber = 4; }
            undo = false;

        });

        createNewGame(pane, gameSet, gameGrid);

        Scene scene = new Scene(gameGrid, WIDTH, HEIGHT);
        stage.setScene(scene);
        stage.show();
    }



    /**
     * Varovne okno upozorni na nespravne pouzitie tlacidiel
     * @return varovne okno
     */
    private Alert clickFirst(){
        Alert alert = new Alert(Alert.AlertType.WARNING);
        alert.setTitle("Game warning");
        alert.setHeaderText("Game location");
        alert.setContentText("First off click on playing field !");

        alert.showAndWait();
        return alert;
    }

    /**
     * Vytvaranie a pridavanie novych hier na spravnu poziciu v hracom poli
     * @param pane      plocha na ktorej sa hra vytvori
     * @param gameSet   nova hra
     * @param gameGrid  mriezka do ktorej bude hra vlozena
     */
    private void createNewGame(Pane pane, GameSet gameSet, GridPane gameGrid) {
        numberOfGame++;
        gamePack(pane, gameSet);
        setEmptyCard(pane, gameSet);
        workingPack(pane, gameSet);
        targetPack(pane, gameSet);
        createNewGame = true;

        if (numberOfGame == 1) {
            gameGrid.add(pane, 0, 1);
            gameSet1 = gameSet;
        } else if (numberOfGame == 2) {
            gameGrid.add(pane, 1, 1);
            gameSet2 = gameSet;
        } else if (numberOfGame == 3) {
            gameGrid.add(pane, 0, 2);
            gameSet3 = gameSet;
        } else if (numberOfGame == 4) {
            gameGrid.add(pane, 1, 2);
            gameSet4 = gameSet;
        }
        pushGameSet(gameSet);
        pane.setPrefSize((WIDTH / 2) - 20, (HEIGHT / 2) - 40);
    }

    /**
     * Vytvorenie spodnych kariet v casti WorkPack
     * @param pane      platno hry
     * @param gameSet   hra
     */
    private void setEmptyCard(Pane pane, GameSet gameSet) {
        for (int i = 0; i < 7; i++) {
            Image workPackEmpty;
            workPackEmpty = backCard2;
            ImageView emptyImage = new ImageView(workPackEmpty);
            emptyImage.setFitWidth(62);
            emptyImage.setPreserveRatio(true);

            emptyImage.setTranslateX(i * 70);
            emptyImage.setTranslateY(110);

            emptyImage.setOnDragOver(targetDragOverHandler());
            emptyImage.setOnDragEntered(targetDragEnteredHandler(emptyImage));
            emptyImage.setOnDragExited(createDragExitedHandler(emptyImage));
            emptyImage.setOnDragDropped(emptyDragDroppedHandler(emptyImage, i, gameSet));

            pane.getChildren().add(emptyImage);
        }
    }

    /**
     * Presuvanie kariet na prazdne policka v casti WorkingPack
     * @param pImageView    obrazok karty
     * @param column        stĺpec z ktoreho bola karta zobrata
     * @param gameSet       hra
     */
    private EventHandler<DragEvent> emptyDragDroppedHandler(final ImageView pImageView, int column, GameSet gameSet) {
        return pEvent -> {

            Dragboard db = pEvent.getDragboard();
            boolean success = false;

            if (db.hasImage()) {
                if (holdingCard.value() == 13) {  // Na prazdne policko iba karta s value 13
                    if (cardFromGamePack) {      // presuvam na prazdne policko workingPack kartu z gamePack
                        gameSet.workPack[column].put(holdingCard);
                        gameSet.workPack[column].top().turnFaceUp();
                        gameSet.dropPack.pop();
                    } else if (cardFromTargetPack && !gameSet.workPack[column].isEmpty() || cardFromWorkingPack) {
                        if (gameSet.workPack[takeFromColumn].top() == holdingCard) {
                            gameSet.workPack[column].put(holdingCard);
                            gameSet.workPack[takeFromColumn].pop(takeFromIndex);
                        } else {
                            gameSet.workPack[column].put(gameSet.workPack[takeFromColumn].pop(takeFromIndex));
                        }
                    }
                }

                restartGame(gameSet);
                pImageView.setImage(db.getImage());
                success = true;
            }

            pEvent.setDropCompleted(success);

            pEvent.consume();

        };
    }


    /**
     * Vytvorenie obrazov jednotlivych kariet ulozenych v triede GamePack
     * @param pane      platno hry
     * @param gameSet   hra
     */
    private void gamePack(Pane pane, GameSet gameSet) {
        if (gameSet.gamePack.size() != 0) {
            for (int i = 0; i < gameSet.gamePack.size(); i++) {
                Image gamePackImage = backCard;

                ImageView packImageView = new ImageView(gamePackImage);
                packImageView.setFitWidth(62);
                packImageView.setPreserveRatio(true);

                packImageView.setTranslateX(0);
                packImageView.setTranslateY(0);

                packImageView.setOnMouseClicked(addEventHandler(gameSet, pane, packImageView));

                pane.getChildren().add(packImageView);
            }
        }
    }

    /**
     * Postupnym klikanim na GamePack su karty odoberane a pridavane do DropPack
     * @param gameSet       hra
     * @param pane          platno hry
     * @param imageView     obrazok karty
     */
    private EventHandler<MouseEvent> addEventHandler(GameSet gameSet, Pane pane, ImageView imageView) {
        return event -> {
            undo = false;
            Card clickedCard = gameSet.gamePack.top();
            Image gamePackImage;
            if (clickedCard != null) {
                gamePackImage = backCard;
                gameSet.dropPack.put(gameSet.gamePack.pop());

            } else {

                gamePackImage = emptyCard;

                while (!gameSet.dropPack.isEmpty()) {
                    gameSet.gamePack.put(gameSet.dropPack.pop());
                }
            }
            ImageView packImageView = new ImageView(gamePackImage);
            packImageView.setFitWidth(62);
            packImageView.setPreserveRatio(true);

            packImageView.setTranslateX(0);
            packImageView.setTranslateY(0);

            clickedCard = position(imageView).dropPack.top();
            dropPacks(clickedCard, pane);


            packImageView.setOnMouseClicked(addEventHandler(gameSet, pane, packImageView));

            pane.getChildren().add(packImageView);

            event.consume();
        };
    }

    /**
     * Podľa drzaneho obrazku funkcia zisťuje v ktorej casti na hracom poli sa nachadza
     * @param pImageView drzany obrazok
     * @return  hru z ktorej bola karta zobrata
     */
    private GameSet position(ImageView pImageView) {
        Bounds localImg = pImageView.localToScene(pImageView.getBoundsInLocal());
        if (localImg.getMinX() < ((WIDTH / 2) - 20) ) {
            if (localImg.getMinY() < (HEIGHT / 2) - 40) {
                clickOnGridNumber = 1;
                return gameSet1;
            }
            if (localImg.getMinY() > (HEIGHT / 2) - 40) {
                clickOnGridNumber = 3;
                return gameSet3;
            }
        } else if (localImg.getMinX() > ((WIDTH / 2) - 20)) {
            if (localImg.getMinY() < (HEIGHT / 2) - 40 ) {
                clickOnGridNumber = 2;
                return gameSet2;
            }
            if (localImg.getMinY() > (HEIGHT / 2) - 40) {
                clickOnGridNumber = 4;
                return gameSet4;
            }
        }
        return gameSet;
    }

    /**
     * Vytvorenie karty po kliknuti na GamePack
     * @param clickedCard   karta ktora sa ma vytvoriť
     * @param pane          platno hry
     */
    private void dropPacks(Card clickedCard, Pane pane) {
        if(clickedCard != null) {

            Image image = getImage(clickedCard.value() + clickedCard.color().toString());
            ImageView imageView = new ImageView(image);
            imageView.setFitWidth(62);
            imageView.setPreserveRatio(true);
            clickedCard.turnFaceUp();
            imageView.setOnDragDetected(createDragDetectedHandler(imageView));
            imageView.setOnDragOver(createDragOverHandler(imageView, gameSet.dropPack.top()));
            imageView.setOnDragEntered(createDragEnteredHandler(imageView, gameSet.dropPack.top()));
            imageView.setOnDragExited(createDragExitedHandler(imageView));
            if (!undo){
                if (clickOnGridNumber == 1) { pushGameSet(gameSet1); }
                else if (clickOnGridNumber == 2) { pushGameSet(gameSet2); }
                else if (clickOnGridNumber == 3) { pushGameSet(gameSet3); }
                else { pushGameSet(gameSet4); }
            }

            imageView.setTranslateX(72);
            imageView.setTranslateY(0);


            pane.getChildren().add(imageView);
        }
    }

    /**
     * Uchopenie karty z DropPack
     * @param pImageView    obrazok karty
     */
    private EventHandler<MouseEvent> createDragDetectedHandler(final ImageView pImageView) {
        return event -> {
            GameSet helpGameSet = position(pImageView);
            holdingCard = helpGameSet.dropPack.top();
            holdingImage = pImageView;
            cardFromTargetPack = false;
            cardFromGamePack = true;

            Dragboard db = pImageView.startDragAndDrop(TransferMode.MOVE);

            ClipboardContent content = new ClipboardContent();

            Image sourceImage = pImageView.getImage();
            content.putImage(sourceImage);
            db.setContent(content);

            event.consume();


        };
    }


    /**
     * Vytvorenie obrazkov k jednotlivym objektom z WorkingPack
     * @param pane      platno hry
     * @param gameSet   hra
     */
    private void workingPack(Pane pane, GameSet gameSet) {
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < gameSet.workPack[i].size(); j++) {

                Image gamePackImage;
                if (gameSet.workPack[i].get(j).isTurnedFaceUp()) {
                    gamePackImage = getImage(gameSet.workPack[i].get(j).value() + gameSet.workPack[i].get(j).color().toString());
                } else {
                    gamePackImage = backCard;
                }

                ImageView workingImageView = new ImageView(gamePackImage);
                workingImageView.setFitWidth(62);
                workingImageView.setPreserveRatio(true);

                workingImageView.setTranslateX(0);
                workingImageView.setTranslateY(0);

                workingImageView.setTranslateX(i * 70);
                workingImageView.setTranslateY(j * 25 + 110);


                workingImageView.setOnDragDetected(createDragDetectedHandler(workingImageView, gameSet.workPack[i].get(j), i, j));
                workingImageView.setOnDragOver(createDragOverHandler(workingImageView, gameSet.workPack[i].get(j)));
                workingImageView.setOnDragEntered(createDragEnteredHandler(workingImageView, gameSet.workPack[i].get(j)));
                workingImageView.setOnDragExited(createDragExitedHandler(workingImageView));
                workingImageView.setOnDragDropped(createDragDroppedHandler(workingImageView, gameSet.workPack[i].get(j), i));

                pane.getChildren().add(workingImageView);
            }
        }
    }

    /**
     * Vytvorenie obrazkov k jednotlivym objektom z TargetPacku
     * @param pane      platno hry
     * @param gameSet   hra
     */
    private void targetPack(Pane pane, GameSet gameSet) {
        for (int i = 0; i < 4; i++) {
            Image targetPackImage;

            if (gameSet.targPack[i].size() == 0) {
                targetPackImage = backCard2;
            } else {
                targetPackImage = getImage(gameSet.targPack[i].top().value() + gameSet.targPack[i].top().color().toString());
            }
            ImageView target = new ImageView(targetPackImage);
            target.setFitWidth(62);
            target.setPreserveRatio(true);

            target.setTranslateX((i + 3) * 70);
            target.setTranslateY(0);
            target.setOnDragDetected(targetDragDetectedHandler(target, gameSet.targPack[i].top(), i));
            target.setOnDragOver(targetDragOverHandler());
            target.setOnDragEntered(targetDragEnteredHandler(target));
            target.setOnDragExited(createDragExitedHandler(target));
            target.setOnDragDropped(targetDragDroppedHandler(i));


            pane.getChildren().add(target);
        }
    }

    /**
     * Nastavenie pohybu karty z target baličku
     */
    private EventHandler<DragEvent> targetDragOverHandler() {
        return pEvent -> {
            pEvent.acceptTransferModes(TransferMode.MOVE);
            pEvent.consume();
        };
    }

    /**
     * Uchopenie karty z target balicka
     * @param pImageView    uchopeny obrazok
     * @param pCard         objekt karty reprezentovany obrazkom
     * @param column        stĺpec na ktorom je karta uchopena
     */
    private EventHandler<MouseEvent> targetDragDetectedHandler(final ImageView pImageView, final Card pCard, int column) {
        return event -> {
            holdingCard = pCard;
            holdingImage = pImageView;
            cardFromTargetPack = true;
            takeFromColumn = column;
            cardFromGamePack = false;
            cardFromWorkingPack = false;

            Dragboard db = pImageView.startDragAndDrop(TransferMode.MOVE);

            ClipboardContent content = new ClipboardContent();

            Image sourceImage = pImageView.getImage();
            content.putImage(sourceImage);
            db.setContent(content);

            event.consume();

        };
    }

    /**
     * Efekt tieňa
     * @param pImageView obrazok nad ktorym sa efekt vykonať
     */
    private EventHandler<DragEvent> targetDragEnteredHandler(final ImageView pImageView) {
        return pEvent -> {
            pImageView.setEffect(new DropShadow());
            pEvent.consume();
        };
    }

    /**
     * Pokladanie karty na target balicek
     * @param column stĺpec nad ktorym je drzana karta 0 - 3
     */
    private EventHandler<DragEvent> targetDragDroppedHandler(int column) {
        return pEvent -> {

            GameSet helpGameSet;

            if (clickOnGridNumber == 1) {
                helpGameSet = gameSet1;
            } else if (clickOnGridNumber == 2) {
                helpGameSet = gameSet2;
            } else if (clickOnGridNumber == 3) {
                helpGameSet = gameSet3;
            } else {
                helpGameSet = gameSet4;
            }
            if (!undo){
                pushGameSet(helpGameSet);
            }
            undo = false;

            if (helpGameSet.workPack[takeFromColumn].top() == holdingCard || cardFromGamePack) {
                if (helpGameSet.targPack[column].put(holdingCard)) {
                    if (!cardFromGamePack) {
                        helpGameSet.workPack[takeFromColumn].pop(takeFromIndex);
                    } else {
                        helpGameSet.dropPack.pop();
                    }
                    restartGame(helpGameSet);
                }
            }

            pEvent.setDropCompleted(true);
            pEvent.consume();

        };
    }

    /**
     * Funkcia sa prevedie po uchopeni karty
     * @param pImageView    uchopeny obrazok
     * @param pCard         objekt karty reprezentovany obrazkom
     * @param column        stĺpec z ktoreho karta pochadza
     * @param index         index karty v stĺpci
     */
    private EventHandler<MouseEvent> createDragDetectedHandler(final ImageView pImageView, final Card pCard, int column, int index) {
        return event -> {
            cardFromTargetPack = false;
            cardFromGamePack = false;
            cardFromWorkingPack = true;
            holdingCard = pCard;
            holdingImage = pImageView;
            takeFromColumn = column;
            takeFromIndex = index;

            if (pCard.isTurnedFaceUp()) {

                Dragboard db = pImageView.startDragAndDrop(TransferMode.MOVE);

                ClipboardContent content = new ClipboardContent();

                Image sourceImage = pImageView.getImage();
                content.putImage(sourceImage);
                db.setContent(content);

                event.consume();
            }
        };
    }

    /**
     * Transformacia obrazku - pohyb karty
     * @param pImageView    obrazok
     * @param pCard         objekt karty
     */
    private EventHandler<DragEvent> createDragOverHandler(final ImageView pImageView, final Card pCard) {
        return pEvent -> {
            if (pEvent.getGestureSource() != pImageView && pEvent.getDragboard().hasString() || (holdingCard.value() + 1) == pCard.value() && pCard.isTurnedFaceUp()) {
                pEvent.acceptTransferModes(TransferMode.MOVE);
            }
            pEvent.consume();
        };
    }

    /**
     * Nastavenie efektu na obrazkom
     * @param pImageView    obrazok na ktorým sa nastaví efekt
     * @param pCard         objekt karty
     */
    private EventHandler<DragEvent> createDragEnteredHandler(final ImageView pImageView, final Card pCard) {
        return pEvent -> {
            if (pCard.isTurnedFaceUp())
                pImageView.setEffect(new DropShadow());
            pEvent.consume();
        };
    }

    /**
     * Zrusenie efektu okolo kary
     * @param pImageView    obrazok naktory efekt pôsobil
     */
    private EventHandler<DragEvent> createDragExitedHandler(final ImageView pImageView) {
        return pEvent -> {
            pImageView.setEffect(null);
            pEvent.consume();
        };
    }


    /**
     * Funkcia sa prevedie v pripade, ze drzana karta je pustena nad kartami z triedy WorkingPack
     * @param pImageView    obrazok karty nad ktorou sa aktualne nachadza mys
     * @param pCard         objekt (karta) ktory je reprezentovany obrazkom pImageView
     * @param column        stľpec hry od 0 - 6
     */
    private EventHandler<DragEvent> createDragDroppedHandler(final ImageView pImageView, final Card pCard, int column) {
        return pEvent -> {

            Dragboard db = pEvent.getDragboard();
            boolean success = false;

            Bounds localImg  = pImageView.localToScene(pImageView.getBoundsInLocal());
            Bounds holdImg = holdingImage.localToScene(holdingImage.getBoundsInLocal());
            boolean moveEnable = false;
            if (localImg.getMinX() < ((WIDTH / 2) - 20) && holdImg.getMinX() < ((WIDTH / 2) - 20)) {
                if (localImg.getMinY() < (HEIGHT / 2) - 40 && holdImg.getMinY() < (HEIGHT / 2) - 40) {
                    moveEnable = true;
                }
                if (localImg.getMinY() > (HEIGHT / 2) - 40 && holdImg.getMinY() > (HEIGHT / 2) - 40) {
                    moveEnable = true;
                }
            } else if (localImg.getMinX() > ((WIDTH / 2) - 20) && holdImg.getMinX() > ((WIDTH / 2) - 20)) {
                if (localImg.getMinY() < (HEIGHT / 2) - 40 && holdImg.getMinY() < (HEIGHT / 2) - 40) {

                    moveEnable = true;
                }
                if (localImg.getMinY() > (HEIGHT / 2) - 40 && holdImg.getMinY() > (HEIGHT / 2) - 40) {
                    moveEnable = true;
                }
            }

            if (db.hasImage() && !pCard.similarColorTo(holdingCard.color()) && moveEnable) {
                GameSet helpGameSet;

                if (clickOnGridNumber == 1) {
                    helpGameSet = gameSet1;
                } else if (clickOnGridNumber == 2) {
                    helpGameSet = gameSet2;
                } else if (clickOnGridNumber == 3) {
                    helpGameSet = gameSet3;
                } else {
                    helpGameSet = gameSet4;
                }
                if (!undo){
                    pushGameSet(helpGameSet);
                }
                undo = false;
                if(pCard == helpGameSet.workPack[column].top()){
                    if (!cardFromGamePack) {
                        if (!cardFromTargetPack) {
                            if (pCard == helpGameSet.workPack[column].top()) {
                                helpGameSet.workPack[column].put(helpGameSet.workPack[takeFromColumn].pop(takeFromIndex));
                            }
                        } else {
                            if (!cardFromWorkingPack) {
                                helpGameSet.targPack[takeFromColumn].pop();
                                helpGameSet.workPack[column].put(holdingCard);
                            } else {
                                helpGameSet.workPack[column].put(helpGameSet.workPack[takeFromColumn].pop(takeFromIndex));
                            }
                        }
                    } else {
                        helpGameSet.dropPack.top().turnFaceUp();
                        helpGameSet.workPack[column].put(helpGameSet.dropPack.top());
                        helpGameSet.dropPack.pop();
                    }
                }
                restartGame(helpGameSet);
                success = true;
            }

            pEvent.setDropCompleted(success);

            pEvent.consume();

        };
    }

    /**
     * Obnovenie hry po presunuti karty
     * @param gameSet obnovovana hra
     */
    private void restartGame(GameSet gameSet) {

        if(clickOnGridNumber == 1){       gameGrid.getChildren().remove(pane1);}
        else if (clickOnGridNumber == 2){ gameGrid.getChildren().remove(pane2);}
        else if (clickOnGridNumber == 3){ gameGrid.getChildren().remove(pane3);}
        else if (clickOnGridNumber == 4){ gameGrid.getChildren().remove(pane4);}

        if (undo || LoadGame || resetGame){
            if(clickOnGridNumber == 1){       gameSet1 = gameSet;}
            else if (clickOnGridNumber == 2){ gameSet2 = gameSet;}
            else if (clickOnGridNumber == 3){ gameSet3 = gameSet;}
            else if (clickOnGridNumber == 4){ gameSet4 = gameSet;}
            resetGame = false;
        }
        Pane pane = new Pane();

        boolean win = true;
        for (int i = 0; i < 4; i++) {
            if (gameSet.targPack[i].size() != 13) {
                win = false;
            }
        }
        if (!win) {
            if (gameSet.dropPack.size() != 0) {
                dropPacks(gameSet.dropPack.top(), pane);
                if (gameSet.gamePack.size() == 0) {
                    while (!gameSet.dropPack.isEmpty()) {
                        gameSet.gamePack.put(gameSet.dropPack.pop());
                    }
                }
            } else {
                if (gameSet.gamePack.size() != 0) {
                    if(gameSet.dropPack.size() != 0){
                        gameSet.dropPack.put(gameSet.gamePack.top());
                        gameSet.gamePack.pop();
                        dropPacks(gameSet.dropPack.top(), pane);
                    }
                }
            }

            gamePack(pane, gameSet);
            setEmptyCard(pane, gameSet);
            workingPack(pane, gameSet);
            targetPack(pane, gameSet);

            wherePut(pane, false);

        } else {
            Image targetPackImage = new Image(new File("lib/pic/win.jpg").toURI().toString());
            ImageView target = new ImageView(targetPackImage);
            target.setFitWidth((WIDTH / 2) - 20);
            target.setFitHeight((HEIGHT / 2) - 40);
            pane.getChildren().add(target);
            wherePut(pane, true);

        }
    }

    /**
     * Ukladanie 6 ťahov na ich spätne vyvolanie pomocou tlacidla undo (←)
     * @param gameSet ukladana hra
     */
    private void pushGameSet(GameSet gameSet){
        GameSet cloneGame = new GameSet();
        cloneGame.initPacks();
        int eqNumber = 0 ;

        for(int j = 0; j < gridStack.size() - 1; j++){
            if(gridStack.get(j) == clickOnGridNumber){
                eqNumber ++;
            }
            // max 6 zaznamov z kazdej hry
            if(eqNumber == 6){
                for(int i = 0; i < gridStack.size() - 1; i++){
                    if (gridStack.get(i) == clickOnGridNumber){
                        gridStack.remove(i);
                        gameStack.remove(i);
                        break;
                    }
                }
            }
        }

        for(int i = 0;i < gameSet.gamePack.size(); i++){
            cloneGame.gamePack.put(gameSet.gamePack.get(i));
        }
        if(gameSet.dropPack.size() != 0) {
            for (int i = 0; i < gameSet.dropPack.size(); i++ ){
                cloneGame.dropPack.put(gameSet.dropPack.get(i));
            }
        }
        for (int i =0; i < 7; i++){
            for (int j = 0; j < gameSet.workPack[i].size(); j++){
                cloneGame.workPack[i].initPut(gameSet.workPack[i].get(j));
            }
        }
        for (int i =0; i < 4; i++){
            for (int j = 0; j < gameSet.targPack[i].size(); j++){
                cloneGame.targPack[i].put(gameSet.targPack[i].get(j));
            }

        }
        if(!createNewGame){
            gridStack.push(clickOnGridNumber);
            gameStack.push(cloneGame);
        } else {
            gridStack.push(numberOfGame);
            gameStack.push(cloneGame);
        }
        createNewGame = false;
    }

    /**
     * Vlozenie novo vytvoreneho platna do mriezky
     * @param pane  nove platno hry
     * @param win   v pripade vyhry je nastaveny na true
     */
    private void wherePut(Pane pane, boolean win) {
        if (clickOnGridNumber == 1) {
            gameGrid.add(pane, 0, 1);
            pane1 = pane;
        } else if (clickOnGridNumber == 2) {
            gameGrid.add(pane, 1, 1);
            pane2 = pane;
        } else if (clickOnGridNumber == 3) {
            gameGrid.add(pane, 0, 2);
            pane3 = pane;
        } else if (clickOnGridNumber == 4) {
            gameGrid.add(pane, 1, 2);
            pane4 = pane;
        }

        if (!win) {
            pane.setStyle("-fx-background-color: #E3E3E3;");
        } else {
            pane.setOpacity(0.7);
        }
        LoadGame = false;
        pane.setPrefSize((WIDTH / 2) - 20, (HEIGHT / 2) - 40);
    }

}