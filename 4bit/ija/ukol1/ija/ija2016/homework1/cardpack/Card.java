package ija.ija2016.homework1.cardpack;

import java.util.Objects;

public class Card
{
	private Card.Color color;
	private int value;

	public Card(Card.Color c, int value)
	{
		this.color = c;
		this.value = value;
	}

	public Card.Color color()
	{
		return color;
	}

	public int 	value()
	{
		return value;
	}

	@Override
	public String toString()
	{

		String name = Integer.toString(value);;
		if (value > 10)
		{
			switch(value)
			{
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
		return color == card.color && value == card.value;
	}

	@Override
	public int hashCode()
	{
		return Objects.hash(color, value);
    }

	public static enum Color {
		CLUBS ("C"),
		DIAMONDS ("D"),
		HEARTS ("H"),
		SPADES ("S");

		private final String name;       

	    private Color(String s) {
	        name = s;
	    }

	    public String toString() {
	       return name;
	    }
	}
}