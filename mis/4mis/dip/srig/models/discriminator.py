import tensorflow as tf
from tensorflow.keras import layers
from .model import Model, downsample

# znxlwm: tensorflow-MNIST-GAN-DCGAN
# Filters: 128 -> 256 -> 512 -> 1024 -> 1
# Kernel: [4, 4]
# Strides: (2,2), posledni (1, 1)
leaky_relu_alpha = 0.2
stddev = 0.025  # viz https://pytorch.org/tutorials/beginner/dcgan_faces_tutorial.html
# viz generator.py
adam_lr = 0.0002
adam_beta1 = 0.5


class Discriminator(Model):
    def __init__(self):
        super().__init__(loss_function=tf.keras.losses.BinaryCrossentropy(from_logits=True),
                         optimizer=tf.keras.optimizers.Adam(learning_rate=adam_lr, beta_1=adam_beta1))

    def save(self):
        pass

    def calculate_loss(self, output_real, output_fake):
        return self.loss_function(tf.ones_like(output_real), output_real) + \
               self.loss_function(tf.zeros_like(output_fake), output_fake)

    def build_model(self):
        """"""
        inp = tf.keras.layers.Input(shape=[256, 256, 3], name='Input_image')
        tar = tf.keras.layers.Input(shape=[256, 256, 3], name='Target_image')
        x = tf.keras.layers.concatenate([inp, tar], name='Concatenate')  # (bs, 256, 256, channels*2)
        down1 = downsample(64, 4, False, name='Downsample_1')(x)  # (bs, 128, 128, 64)
        down2 = downsample(128, 4, name='Downsample_2')(down1)  # (bs, 64, 64, 128)
        down3 = downsample(256, 4, name='Downsample_3')(down2)  # (bs, 32, 32, 256)
        down4 = downsample(512, 4, strides=1, name='Downsample_4', zero_padding=True)(down3)  # (bs, 31, 31, 512)
        down5 = downsample(1, 4, strides=1, name='Downsample_5', zero_padding=True, apply_batchnorm=False, leaky_relu=False)(down4)  # (bs, 30, 30, 1)
        return tf.keras.Model(inputs=[inp, tar], outputs=down5)
