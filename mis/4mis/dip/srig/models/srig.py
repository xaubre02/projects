from .generator import Generator
from .discriminator import Discriminator

import os
import sys
import time
import datetime
import tensorflow as tf
import matplotlib.pyplot as plt
from pathlib import Path


class SRIG:

    def __init__(self, log_directory=None):
        self.logdir = self.create_log_directory(log_directory)

        self.generator = Generator()
        self.discriminator = Discriminator()

        # self.checkpoint_prefix = os.path.join('./checkpoints', 'srig')  # TODO: predelat na Path z pathlibu
        self.checkpoint_prefix = str(self.logdir / 'checkpoints/srig')
        self.checkpoint = tf.train.Checkpoint(generator_optimizer=self.generator.optimizer,
                                              discriminator_optimizer=self.discriminator.optimizer,
                                              generator=self.generator.model,
                                              discriminator=self.discriminator.model)
        # summary writer
        self.summary_writer = tf.summary.create_file_writer(str(self.logdir / 'summary'))

    @staticmethod
    def create_log_directory(path):
        if path is None:
            # current working directory / logs
            path = Path.cwd() / 'logs'
        else:
            if isinstance(path, str):
                path = Path(path)
            # relative path
            if not path.is_absolute():
                path = Path.cwd() / path
                path = path.resolve()
        # create the log directory
        if not path.exists():
            print('Creating logging directory: {}'.format(path))
            path.mkdir(parents=True)

        return path

    @tf.function  # This annotation causes the function to be "compiled".
    def train_step(self, input_image, target, epoch):
        with tf.GradientTape() as gen_tape, tf.GradientTape() as disc_tape:
            g_img = self.generator.model(input_image, training=True)

            real_output = self.discriminator.model([input_image, target], training=True)
            fake_output = self.discriminator.model([input_image, g_img], training=True)

            gen_total_loss, gen_gan_loss, gen_l1_loss = self.generator.calculate_loss(fake_output, g_img, target)
            disc_loss = self.discriminator.calculate_loss(real_output, fake_output)

        self.generator.update_weights(gen_tape.gradient(gen_total_loss, self.generator.trainable_variables))
        self.discriminator.update_weights(disc_tape.gradient(disc_loss, self.discriminator.trainable_variables))

        with self.summary_writer.as_default():
            tf.summary.scalar('gen_total_loss', gen_total_loss, step=epoch)
            tf.summary.scalar('gen_gan_loss', gen_gan_loss, step=epoch)
            tf.summary.scalar('gen_l1_loss', gen_l1_loss, step=epoch)
            tf.summary.scalar('disc_loss', disc_loss, step=epoch)

    def train(self, train_data, test_data, checkpoint_period, output_period, epochs):
        # image output directory
        outdir = self.logdir / 'output'
        if not outdir.exists():
            outdir.mkdir(parents=True)

        print('Training data count: {}'.format(len(list(train_data))))
        print('Testing data count:  {}'.format(len(list(test_data))))
        print()

        print('Training..\n')
        steps_per_epoch = len(list(train_data.enumerate()))
        training_start = time.time()
        for epoch in range(epochs):
            start = time.time()

            print('Epoch #{:d}'.format(epoch + 1))
            if output_period > 0 and (epoch + 1) % output_period == 0:
                # generate an image
                print('Generating output..')
                first_img_pair = list(test_data.as_numpy_iterator())[0]
                output_file = outdir / 'image_at_epoch_{:03d}.jpg'.format(epoch + 1)
                self.generate_and_save_image(generator=self.generator.model,
                                             input_img=first_img_pair[0],
                                             target=first_img_pair[1],
                                             output_file=output_file)

            # Train
            for n, (input_image, target) in train_data.enumerate():
                sys.stdout.write('\rCompleted: {:d}% (step: [{:d}/{:d}])'.format(int((n+1) / steps_per_epoch * 100), n+1, steps_per_epoch))
                self.train_step(input_image, target, epoch)
            print()

            # save the checkpoint every 'checkpoint_period' epochs
            if checkpoint_period > 0 and (epoch + 1) % checkpoint_period == 0:
                print('Saving checkpoint..')
                self.checkpoint.save(file_prefix=self.checkpoint_prefix)

            print('Time: {:d} seconds'.format(int(time.time() - start)))
            print('-------------------------------\n')

        # save the last checkpoint
        print('Saving the last checkpoint..')
        self.checkpoint.save(file_prefix=self.checkpoint_prefix)
        print('==============================================================')
        total_time = time.time() - training_start
        print('Training took {:.2f} minutes'.format(total_time / 60))
        print('Average time per epoch: {:d} seconds'.format(int(total_time / epochs)))

    @staticmethod
    def generate_and_save_image(generator, input_img, target, output_file, plot=False):
        prediction = generator(input_img, training=True)  # TODO: co s trainingem?
        # getting the pixel values between [0, 1] to plot it.
        p_img = prediction[0] * 0.5 + 0.5
        plt.imsave(str(output_file), p_img.numpy())

        # plot images
        if plot:
            display_list = [input_img[0], target[0], prediction[0]]
            title = ['Input Image', 'Ground Truth', 'Predicted Image']
            for i in range(3):
                plt.subplot(1, 3, i + 1)
                plt.title(title[i])
                # getting the pixel values between [0, 1] to plot it.
                plt.imshow(display_list[i] * 0.5 + 0.5)
                plt.axis('off')
            plt.show()
