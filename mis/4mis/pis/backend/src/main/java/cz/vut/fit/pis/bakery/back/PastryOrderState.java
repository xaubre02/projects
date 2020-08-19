package cz.vut.fit.pis.bakery.back;

public enum PastryOrderState {
	Created {
		@Override
		public String toString() {
			return "created";
		}
	},
	Prepared {
		@Override
		public String toString() {
			return "prepared";
		}
	},
	Delivered {
		@Override
		public String toString() {
			return "delivered";
		}
	},
	Canceled {
		@Override
		public String toString() {
			return "canceled";
		}
	};

	public static PastryOrderState getState(String stateStr) {
		switch (stateStr) {
			case "created":
				return PastryOrderState.Created;
			case "prepared":
				return PastryOrderState.Prepared;
			case "delivered":
				return PastryOrderState.Delivered;
			case "canceled":
				return PastryOrderState.Canceled;
			default:
				return null;
		}
	}
}
