import tensorflow as tf
from tensorflow.keras import layers

from .model import Model, downsample, upsample

#TODO: checkout tento generator: https://towardsdatascience.com/dcgans-generating-dog-images-with-tensorflow-and-keras-fb51a1071432

# TODO: Do diplomky: Loss function: With D and G setup, we can specify how they learn through the loss functions and optimizers. We will use the Binary Cross Entropy loss (BCELoss) function which is defined in PyTorch as: vzorecek
# TODO; zdroj zase pytorch
# TODO: Adam optimizers with learning rate 0.0002 and Beta1 = 0.5 according to the DCGAN paper
        # optimizerD = optim.Adam(netD.parameters(), lr=lr, betas=(beta1, 0.999))
        # je tam moc dobře popsaná implementace
adam_lr = 0.0002  # 0.008 je v deep with python, decay=1e-8)
adam_beta1 = 0.5

# znxlwm: tensorflow-MNIST-GAN-DCGAN
# Filters: 1024 -> 512 -> 256 -> 128 -> 1
# Kernel: [4, 4]
# Strides: (1,1), potom (2, 2)
leaky_relu_alpha = 0.2
trainable = True   # odkaz na dobrej popis výpočtu batch normalizace: https://www.tensorflow.org/api_docs/python/tf/keras/layers/BatchNormalization
                   # něco k batch normalizaci: https://arxiv.org/abs/1502.03167
epsilon = 0.0005

#DECONV:
#weights_initializer=tf.truncated_normal_initializer(stddev=stddev),
#            biases_initializer=tf.zeros_initializer(),
# ke každé vrstvě jebnout jméno

stddev = 0.025  # viz https://pytorch.org/tutorials/beginner/dcgan_faces_tutorial.html  TODO: pouzit generator bez dense vrstvy

# TODO: pridat l2 regularizer: https://www.tensorflow.org/tutorials/keras/overfit_and_underfit#combined_l2_dropout
# TODO: - Many models train better if you gradually reduce the learning rate during training. Use optimizers.schedules to reduce the learning rate over time
OUTPUT_CHANNELS = 3


# batchnorm: momentum =0.5 u toho týpka s retinal dcgan
# kernel_initializer='glorot_uniform',
# optimizer = Adam(lr=0.0002, beta_1=0.5)
class Generator(Model):
    def __init__(self):
        super().__init__(loss_function=tf.keras.losses.BinaryCrossentropy(from_logits=True),
                         optimizer=tf.keras.optimizers.Adam(learning_rate=adam_lr, beta_1=adam_beta1))

    def save(self):
        pass

    def calculate_loss(self, disc_generated_output, gen_output, target):
        LAMBDA = 100  # TODO: integrate...
        gan_loss = self.loss_function(tf.ones_like(disc_generated_output), disc_generated_output)
        # mean absolute error
        l1_loss = tf.reduce_mean(tf.abs(target - gen_output))
        total_gen_loss = gan_loss + (LAMBDA * l1_loss)
        return total_gen_loss, gan_loss, l1_loss

    def build_model(self):
        down_stack = [
            downsample(64, 4, apply_batchnorm=False),  # (bs, 128, 128, 64)
            downsample(128, 4),  # (bs, 64, 64, 128)
            downsample(256, 4),  # (bs, 32, 32, 256)
            downsample(512, 4),  # (bs, 16, 16, 512)
            downsample(512, 4),  # (bs, 8, 8, 512)
            downsample(512, 4),  # (bs, 4, 4, 512)
            downsample(512, 4),  # (bs, 2, 2, 512)
            downsample(512, 4),  # (bs, 1, 1, 512)
        ]
        up_stack = [
            upsample(512, 4, apply_dropout=True),  # (bs, 2, 2, 1024)
            upsample(512, 4, apply_dropout=True),  # (bs, 4, 4, 1024)
            upsample(512, 4, apply_dropout=True),  # (bs, 8, 8, 1024)
            upsample(512, 4),  # (bs, 16, 16, 1024)
            upsample(256, 4),  # (bs, 32, 32, 512)
            upsample(128, 4),  # (bs, 64, 64, 256)
            upsample(64, 4),  # (bs, 128, 128, 128)
        ]

        last = upsample(3, 4, batch_norm=False, relu=False, activation='tanh', use_bias=True)  # (bs, 256, 256, 3)

        inputs = tf.keras.layers.Input(shape=[256, 256, 3])
        x = inputs

        # Downsampling through the model
        skips = []
        for down in down_stack:
            x = down(x)
            skips.append(x)

        skips = reversed(skips[:-1])

        # Upsampling and establishing the skip connections
        for up, skip in zip(up_stack, skips):
            x = up(x)
            x = tf.keras.layers.Concatenate()([x, skip])

        x = last(x)

        return tf.keras.Model(inputs=inputs, outputs=x)
