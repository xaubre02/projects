package ija.ija2016.homework2.model.board;

import ija.ija2016.homework2.model.board.AbstractFactorySolitaire;
import ija.ija2016.homework2.model.cards.ready.ImCardStack;
import ija.ija2016.homework2.model.cards.ready.ImCardDeck;
import ija.ija2016.homework2.model.cards.ready.ImCard;
import ija.ija2016.homework2.model.cards.CardStack;
import ija.ija2016.homework2.model.cards.CardDeck;
import ija.ija2016.homework2.model.cards.Card;

public class FactoryKlondike extends AbstractFactorySolitaire {
	public FactoryKlondike() {

	}

	public Card createCard(Card.Color color, int value) {
		if (value < 1 || value > 13 || !Card.Color.contains(color))
			{
				return null;
			}
		
		return new ImCard(color, value);
	}
	
	public CardDeck createCardDeck() {
		return ImCardDeck.createStandardDeck();
	}

	public CardDeck createTargetPack(Card.Color color) {
		return new ImCardDeck(color);
	}

	public CardStack createWorkingPack() {
		return new ImCardStack(true);
	}
}