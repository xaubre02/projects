import os
import sys
import tensorflow as tf
from models import SRIG, Generator, Discriminator
from input_data import get_input_data
from data_loader import DataLoader, InputException
from matplotlib import pyplot as plt



# TODO: move to SRIG class
gpus = tf.config.experimental.list_physical_devices('GPU')
if gpus:
    try:
        # Currently, memory growth needs to be the same across GPUs
        for gpu in gpus:
            tf.config.experimental.set_memory_growth(gpu, True)
    except RuntimeError as e:
        # Memory growth must be set before GPUs have been initialized
        print(e)


def main():
    EPOCHS = 100
    srig = SRIG()
    dl = DataLoader()
    # train_dataset, test_dataset = get_input_data()
    training_dataset = dl.load_dataset('data/training_set', augment=True)
    testing_dataset = dl.load_dataset('data/training_set', augment=False)
    srig.train(training_dataset, EPOCHS, testing_dataset)


if __name__ == '__main__':
    # main()

    dl = DataLoader()
    dl.create_input_images('tmp/drive/input', 'tmp/drive/target', 'data/drive') # TODO: working
