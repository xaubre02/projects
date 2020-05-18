from .generator import Generator
from .discriminator import Discriminator

import os
import time
import datetime
import tensorflow as tf
import matplotlib.pyplot as plt


class SRIG:

    def __init__(self):
        self.generator = Generator()
        self.discriminator = Discriminator()

        self.checkpoint_prefix = os.path.join('./checkpoints', 'srig')  # TODO: predelat na Path z pathlibu
        self.checkpoint = tf.train.Checkpoint(generator_optimizer=self.generator.optimizer,
                                              discriminator_optimizer=self.discriminator.optimizer,
                                              generator=self.generator.model,
                                              discriminator=self.discriminator.model)
        # summary writer TODO: what does it do?
        self.summary_writer = tf.summary.create_file_writer('./summary/' + datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))

        # Define the training loop
        self.BATCH_SIZE = 256
        self.noise_dim = 100
        self.num_examples_to_generate = 16

        # We will reuse this seed overtime (so it's easier)
        # to visualize progress in the animated GIF)
        self.seed = tf.random.normal([self.num_examples_to_generate, self.noise_dim])  # TODO: make really random, like random(time.time())

        self.dlos = []
        self.glos = []

    @tf.function  # This annotation causes the function to be "compiled".
    def train_step(self, input_image, target, epoch):
        with tf.GradientTape() as gen_tape, tf.GradientTape() as disc_tape:
            g_img = self.generator.model(input_image, training=True)

            real_output = self.discriminator.model([input_image, target], training=True)
            fake_output = self.discriminator.model([input_image, g_img], training=True)

            gen_total_loss, gen_gan_loss, gen_l1_loss = self.generator.calculate_loss(fake_output, g_img, target)
            disc_loss = self.discriminator.calculate_loss(real_output, fake_output)
            self.glos.append(gen_total_loss)
            self.dlos.append(disc_loss)

        self.generator.update_weights(gen_tape.gradient(gen_total_loss, self.generator.trainable_variables))
        self.discriminator.update_weights(disc_tape.gradient(disc_loss, self.discriminator.trainable_variables))

        with self.summary_writer.as_default():
            tf.summary.scalar('gen_total_loss', gen_total_loss, step=epoch)
            tf.summary.scalar('gen_gan_loss', gen_gan_loss, step=epoch)
            tf.summary.scalar('gen_l1_loss', gen_l1_loss, step=epoch)
            tf.summary.scalar('disc_loss', disc_loss, step=epoch)

    def train(self, dataset, epochs, dataset_test):
        training_start = time.time()
        for epoch in range(epochs):
            start = time.time()

            # TODO: probably not necessary
            # get first image from the testing set
            first_img_pair = list(dataset_test.as_numpy_iterator())[0]
            self.generate_and_save_images(self.generator.model, first_img_pair[0], first_img_pair[1], epoch)

            print("Epoch: ", epoch)
            print('Training on {:d} images'.format(len(list(dataset))))

            # Train
            for n, (input_image, target) in dataset.enumerate():
                print('.', end='')
                if (n + 1) % 100 == 0:
                    print()
                self.train_step(input_image, target, epoch)
            print()

            # saving (checkpoint) the model every 5 epochs
            if (epoch + 1) % 5 == 0:
                self.checkpoint.save(file_prefix=self.checkpoint_prefix)

            print('Time taken for epoch {} is {} sec\n'.format(epoch + 1, time.time() - start))

        print('Training took {:2f} minutes'.format((time.time() - training_start) / 60))

        self.checkpoint.save(file_prefix=self.checkpoint_prefix)

        # TODO: plot loss
        # plt.figure(figsize=(10, 5))
        # plt.title("Generator and Discriminator Loss During Training")
        # plt.plot(self.glos, label="Gen")
        # plt.plot(self.dlos, label="Dis")
        # plt.xlabel("iterations")
        # plt.ylabel("Loss")
        # plt.legend()
        # plt.show()
        # Generate after the final epoch
        # display.clear_output(wait=True)

    @staticmethod
    def generate_and_save_images(model, test_input, target, epoch, plot=False):
        prediction = model(test_input, training=True)  # TODO: co s trainingem?
        # getting the pixel values between [0, 1] to plot it.
        p_img = prediction[0] * 0.5 + 0.5
        plt.imsave('./output/image_at_epoch_{:03d}.png'.format(epoch), p_img.numpy())

        # plot images
        if plot:
            display_list = [test_input[0], target[0], prediction[0]]
            title = ['Input Image', 'Ground Truth', 'Predicted Image']
            for i in range(3):
                plt.subplot(1, 3, i + 1)
                plt.title(title[i])
                # getting the pixel values between [0, 1] to plot it.
                plt.imshow(display_list[i] * 0.5 + 0.5)
                plt.axis('off')
            plt.show()
