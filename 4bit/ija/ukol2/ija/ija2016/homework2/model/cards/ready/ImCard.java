package ija.ija2016.homework2.model.cards.ready;

import ija.ija2016.homework2.model.cards.Card;
import java.util.Objects;

public class ImCard implements Card {
	private Card.Color color;
	private int value;
	private boolean faceUp;

	public ImCard(Card.Color color, int value) {
		this.color = color;
		this.value = value;
		this.faceUp = false;
	}

	public Card.Color color() {
		return color;
	}

	public int value() {
		return value;
	}

	public boolean isTurnedFaceUp() {
		return faceUp ? true : false;
	}

	public boolean turnFaceUp() {
		if (faceUp)
		{
			return false;
		}
		else
		{
			faceUp = true;
			return true;
		}

	}

	public boolean similarColorTo(Card c) {
		if ( color.equals(Card.Color.CLUBS) || color.equals(Card.Color.SPADES) ){
			return ( c.color().equals(Card.Color.CLUBS) || c.color().equals(Card.Color.SPADES) );
		}
		else {
			return ( c.color().equals(Card.Color.HEARTS) || c.color().equals(Card.Color.DIAMONDS) );
		}
	}

	public boolean similarColorTo(Card.Color c) {
		if ( color.equals(Card.Color.CLUBS) || color.equals(Card.Color.SPADES) ){
			return ( c.equals(Card.Color.CLUBS) || c.equals(Card.Color.SPADES) );
		}
		else {
			return ( c.equals(Card.Color.HEARTS) || c.equals(Card.Color.DIAMONDS) );
		}
	}

	public int compareValue(Card c)
	{
		return value - c.value();
	}

	@Override
	public String toString() {

		String name = Integer.toString(value);;
		if (value > 10 || value == 1) {
			switch(value) {
				case 01:
					name = "A";
					break;

				case 11:
					name = "J";
					break;

				case 12:
					name = "Q";
					break;

				case 13:
					name = "K";
					break;

				default:
					break;
			}
		}

		return name + "(" + color.toString() + ")";
	}

	@Override
	public boolean equals(Object o)
	{
		if (o == this)
			return true;
		if (!(o instanceof Card))
			return false;

		Card card = (Card) o;
		return color == card.color() && value == card.value();
	}

	@Override
	public int hashCode()
	{
		return Objects.hash(color, value);
    }
}