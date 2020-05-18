import tensorflow as tf
from abc import ABCMeta, abstractmethod


class Model(metaclass=ABCMeta):

    def __init__(self, loss_function, optimizer):
        """"""
        self._model = self.build_model()
        self._loss_function = loss_function
        self._optimizer = optimizer

    @property
    def model(self):
        """"""
        return self._model

    @property
    def loss_function(self):
        """"""
        return self._loss_function

    @property
    def optimizer(self):
        """"""
        return self._optimizer

    @property
    def trainable_variables(self):
        return self.model.trainable_variables

    @abstractmethod
    def save(self):
        """"""
        pass

    @abstractmethod
    def calculate_loss(self, *args):
        pass

    @abstractmethod
    def build_model(self):
        pass

    # TODO: does it working?
    def update_weights(self, gradients):
        self.optimizer.apply_gradients(zip(gradients, self.trainable_variables))

    # TODO: implement __call__ function


def downsample(filters, size, apply_batchnorm=True, leaky_relu=True, name='', zero_padding=False, strides=2):
    result = tf.keras.Sequential(name=name)

    if zero_padding:
        result.add(tf.keras.layers.ZeroPadding2D())

    padding = 'valid' if zero_padding else 'same'
    result.add(tf.keras.layers.Conv2D(filters, size, strides=strides, padding=padding, kernel_initializer=tf.initializers.RandomNormal(stddev=0.025), use_bias=False))

    if apply_batchnorm:
        result.add(tf.keras.layers.BatchNormalization())

    if leaky_relu:
        result.add(tf.keras.layers.LeakyReLU())

    return result


def upsample(filters, size, apply_dropout=False, batch_norm=True, relu=True, use_bias=False, activation=None):
    result = tf.keras.Sequential()
    result.add(tf.keras.layers.Conv2DTranspose(filters, size,
                                               strides=2,
                                               padding='same',
                                               activation=activation,
                                               kernel_initializer=tf.initializers.RandomNormal(stddev=0.025),
                                               use_bias=use_bias))

    if batch_norm:
        result.add(tf.keras.layers.BatchNormalization())

    if apply_dropout:
        result.add(tf.keras.layers.Dropout(0.5))

    if relu:
        result.add(tf.keras.layers.ReLU())

    return result
