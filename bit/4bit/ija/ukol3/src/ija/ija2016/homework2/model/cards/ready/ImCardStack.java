package ija.ija2016.homework2.model.cards.ready;

import ija.ija2016.homework2.model.cards.CardStack;
import ija.ija2016.homework2.model.cards.CardDeck;
import ija.ija2016.homework2.model.cards.Card;
import java.util.ArrayList;
import java.util.List;

public class ImCardStack implements CardStack {
	private ArrayList<Card> stack;
	private boolean workingPack;
	private Card.Color color;
	private int value;

	public ImCardStack(boolean type) {
		this.workingPack = type;
		this.value = 13;
		this.stack = new ArrayList<Card>();
	}
	
	public int size() {
		return stack.size();
	}

	public boolean put(Card card) {
		if (workingPack){
			if(card.value() != this.value) return false;
			
			if(color != null){
				if (card.similarColorTo(color)) return false;
			}

			color = card.color();
			value--;
		}
		return stack.add(card);
	}

	public boolean put(CardStack stack) {
		if (workingPack){
			ArrayList<Card> tmp = new ArrayList<Card>();
			
			int lol = stack.size();

			for(int i = 0; i < lol; i++){
				if(!this.put(stack.get(i))) return false;
			}
			return true;
		}

		return this.stack.addAll(stack.toList());
	}

	public boolean put(List<Card> stack) {
		return this.stack.addAll(stack);
	}

	public Card pop() {
		if (stack.isEmpty()) 
			return null;

		int last = stack.size() - 1;
		Card top = stack.get(last);
		stack.remove(last);
		return top;
	}

	public CardStack pop(Card card) {
		if (stack.contains(card)) {
			// pozice hledane karty
			int index = stack.indexOf(card);
			int size = stack.size();

			ImCardStack taken = new ImCardStack(false);
			// prida dane karty do zasobniku taken
			taken.put(stack.subList(index, size));
			// smaze dane karty
			stack.subList(index, size).clear();
			
			return taken;
		}
		else
			return null;
	}

	public Card get() {
		if (stack.isEmpty()) 
			return null;

		return stack.get( stack.size() - 1);
	}

	public Card get(int index) {
		if (stack.isEmpty() || index < 0 || index >= stack.size()) 
			return null;

		return stack.get(index);
	}

	public boolean isEmpty() {
		return stack.isEmpty();
	}

	public List<Card> toList() {
		return stack.subList(0, this.stack.size());
	} 

	@Override
	public boolean equals(Object o) {
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