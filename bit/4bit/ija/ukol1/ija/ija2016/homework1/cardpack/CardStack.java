package ija.ija2016.homework1.cardpack;

import ija.ija2016.homework1.cardpack.Card;
import java.util.ArrayList;
import java.util.List;

public class CardStack
{
	private ArrayList<Card> stack;

	public CardStack(int size)
	{
		this.stack = new ArrayList<Card>(size);
	}

	public void put(Card card)
	{
		this.stack.add(card);
	}

	public void put(CardStack stack)
	{
		this.stack.addAll(stack.convertToList());
	}

	public void put(List<Card> stack)
	{
		this.stack.addAll(stack);
	}

	public List<Card> convertToList()
	{
		return this.stack.subList(0, this.stack.size());
	} 

	public boolean isEmpty()
	{
		return this.stack.isEmpty();
	}

	public int size()
	{
		return this.stack.size();
	}

	public CardStack takeFrom(Card card)
	{
		if (this.stack.contains(card))
		{
			// pozice hledane karty
			int index = this.stack.indexOf(card);
			int size = this.stack.size();

			CardStack taken = new CardStack(size - index);
			// prida dane karty do zasobniku taken
			taken.put(this.stack.subList(index, size));
			// smaze dane karty
			this.stack.subList(index, size).clear();
			
			return taken;
		}
		else
			return null;	
	}

	public Card get(int index)
	{
		return this.stack.get(index);
	}

	@Override
	public boolean equals(Object o)
	{
		if (o == this)
			return true;
		if (!(o instanceof CardStack))
			return false;

		CardStack stack = (CardStack) o;
		int size = this.size();
		for(int i = 0; i < size; i++)
		{
			if(!this.stack.get(i).equals(stack.get(i)))
				return false;
		}
		return true;
	}
}