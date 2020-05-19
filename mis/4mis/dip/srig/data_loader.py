from pathlib import Path
import tensorflow as tf
from PIL import Image
import sys
import numpy


class InputException(Exception):
    """The exception raised during input processing."""

    def __init__(self, msg):
        """Initialize the exception."""
        super().__init__(msg)


class DataLoader:

    SUPPORTED_EXTENSIONS = [
        '.gif',  # TODO: rly? test it
        '.jpg',
        '.png',
        '.tif',
        '.gif'
    ]

    @staticmethod
    def augment(input_image, target_image):
        # resizing to 286 x 286 x 3
        # TODO: parametrize widht and height, rly?
        input_image = DataLoader.resize(input_image, 286)
        target_image = DataLoader.resize(target_image, 286)

        # randomly cropping to 256 x 256 x 3
        stacked_image = tf.stack([input_image, target_image], axis=0)
        cropped_image = tf.image.random_crop(stacked_image, size=[2, 256, 256, 3])
        input_image = cropped_image[0]
        target_image = cropped_image[1]

        # random mirroring
        if tf.random.uniform(()) > 0.5:
            input_image = tf.image.flip_left_right(input_image)
            target_image = tf.image.flip_left_right(target_image)

        return input_image, target_image

    @staticmethod
    def resize(image, size=256):
        # TODO: parametrize widht and height, rly?
        return tf.image.resize(image, [size, size], method=tf.image.ResizeMethod.NEAREST_NEIGHBOR)

    @staticmethod
    def normalize(image):
        # normalizes an images to <-1,1>
        return (image / 127.5) - 1

    @ staticmethod
    @tf.function
    def preprocess_images(input_image, target_image, augment=True):
        # augment the images
        if augment:
            input_image, target_image = DataLoader.augment(input_image, target_image)
        # resize the images
        else:
            input_image = DataLoader.resize(input_image)
            target_image = DataLoader.resize(target_image)

        # normalize the images
        input_image = DataLoader.normalize(input_image)
        target_image = DataLoader.normalize(target_image)

        return input_image, target_image

    @staticmethod
    def load_image(file, ext='.jpg'):
        image = tf.io.read_file(file)        # read the file
        if ext == '.jpg':                    # decode data
            image = tf.image.decode_jpeg(image)
        elif ext == '.png':
            image = tf.image.decode_png(image, channels=3)
        elif ext == '.gif':
            image = tf.image.decode_gif(image)
            image = image[0]  # reshape from (1, x_dim, y_dim, 3) to (x_dim, y_dim, 3)
        image = tf.cast(image, tf.float32)   # cast the image (32b float)
        return image

    @staticmethod
    def split_image(image):
        w = tf.shape(image)[1]
        w = w // 2
        left_side = image[:, :w, :]
        right_side = image[:, w:, :]
        return left_side, right_side

    @staticmethod
    def load_pair_default(file):
        image = DataLoader.load_image(file)
        input_image, target_image = DataLoader.split_image(image)
        return DataLoader.preprocess_images(input_image, target_image, augment=False)

    @staticmethod
    def load_pair_augmented(file):
        image = DataLoader.load_image(file)
        input_image, target_image = DataLoader.split_image(image)
        return DataLoader.preprocess_images(input_image, target_image, augment=True)

    @staticmethod
    def create_dataset(files, augment, batch_size):
        dataset = tf.data.Dataset.list_files(list(map(lambda x: str(x), files)))
        dataset = dataset.map(DataLoader.load_pair_augmented if augment else DataLoader.load_pair_default,
                              num_parallel_calls=tf.data.experimental.AUTOTUNE)
        dataset = dataset.shuffle(500)
        dataset = dataset.batch(batch_size)
        # # TODO: cache
        dataset.cache()  # train_dataset.cache('dir')
        return dataset

    @staticmethod
    def load_dataset(images_path, augment=False, batch_size=1):
        # check the batch size
        if batch_size < 1:
            batch_size = 1

        # process paths
        images_path = DataLoader.process_path(images_path)
        # find all supported images
        input_files = DataLoader.get_images(images_path, extensions=['.jpg'])
        if len(input_files) == 0:
            raise InputException('\'{}\' does not contain any images'.format(images_path))

        return DataLoader.create_dataset(input_files, augment=augment, batch_size=batch_size)

    @staticmethod
    def get_pairs(input_files, target_files):
        pair_list = []
        for ifile in input_files:
            for tfile in target_files:
                # same filename
                if ifile.stem == tfile.stem:
                    pair_list.append([str(ifile), str(tfile)])
                    break
        return pair_list

    @staticmethod
    def get_images(path, extensions=None):
        if extensions is None:
            extensions = DataLoader.SUPPORTED_EXTENSIONS
        images = []
        # list the directory
        for item in path.iterdir():
            # append only supported images
            if item.is_file():
                if item.suffix.lower() in extensions:
                    images.append(item)
            # recursively search the directory
            elif item.is_dir():
                images += DataLoader.get_images(item)
        return images

    @staticmethod
    def process_path(path, dir_check=True):
        # convert string to a Path instance
        if isinstance(path, str):
            path = Path(path)

        # get the full path
        if not path.is_absolute():
            path = Path.cwd() / path
            path = path.resolve()

        if not path.exists():
            raise InputException('\'{}\' does not exist'.format(path))

        if dir_check and not path.is_dir():
            raise InputException('\'{}\' is not a directory'.format(path))

        return path

    @staticmethod
    def concat_images(img1_src, img2_src, output):
        img1 = Image.open(img1_src)
        img2 = Image.open(img2_src)

        if img1.size != img2.size:
            print('Warning: images have different size', file=sys.stderr)
            return None

        if img1.size[0] != img1.size[1]:
            print('Warning: image height and width do not match. Resizing...', file=sys.stderr)

        max_dim = max(img1.size[0], img1.size[1])
        offset_x = int(0 if img1.size[0] == max_dim else (max_dim - img1.size[0]) / 2)
        offset_y = int(0 if img1.size[1] == max_dim else (max_dim - img1.size[1]) / 2)

        size = (max_dim * 2, max_dim)
        new_img = Image.new('RGB', size)
        new_img.paste(img1, (offset_x, offset_y))
        new_img.paste(img2, (max_dim + offset_x * 3, offset_y))

        new_img.save(output)

    @staticmethod
    def process_output_dir(outdir):
        # output directory is not specified
        if outdir is None:
            outdir = Path.cwd() / 'output'
        else:
            if isinstance(outdir, str):
                outdir = Path(outdir)
            # output outdir is not absolute
            if not outdir.is_absolute():
                outdir = Path.cwd() / outdir
                outdir = outdir.resolve()

        if not outdir.exists():
            outdir.mkdir(parents=True)

        return outdir

    @staticmethod
    def create_input_images(input_data_dir, target_data_dir, output_dir):
        # process paths
        input_data_dir = DataLoader.process_path(input_data_dir)
        target_data_dir = DataLoader.process_path(target_data_dir)

        # find all supported images
        input_files = DataLoader.get_images(input_data_dir)
        target_files = DataLoader.get_images(target_data_dir)
        if len(input_files) == 0:
            raise InputException('\'{}\' does not contain any images'.format(input_data_dir))
        if len(target_files) == 0:
            raise InputException('\'{}\' does not contain any images'.format(target_data_dir))

        # find corresponding pairs
        pairs = DataLoader.get_pairs(input_files, target_files)
        if len(pairs) == 0:
            raise InputException('no corresponding (input, target) pairs were found'.format(input_data_dir))

        output_dir = DataLoader.process_output_dir(output_dir)

        pairs_count = len(pairs)
        print('Found {:d} pairs'.format(pairs_count))
        print('Generating new images..')
        print('-----------------------')
        for index, pair in enumerate(pairs):
            print('Generating image [{}/{}]'.format(index + 1, pairs_count))
            output = str(output_dir / 'image{:03d}.jpg'.format(index))
            DataLoader.concat_images(pair[0], pair[1], output=output)

    @staticmethod
    def load_images(path):
        def load_single_image(file):
            suffix = file.suffix.lower()
            if suffix in ['.jpg', '.png', '.gif']:
                image = DataLoader.load_image(str(file), ext=suffix)
                image = DataLoader.resize(image)
                image = DataLoader.normalize(image)
                return image
            # not supported
            return None

        images = []
        path = DataLoader.process_path(path, dir_check=False)
        # single file
        if path.is_file():
            img = load_single_image(path)
            if img is not None:
                images.append(img)
        # directory
        elif path.is_dir():
            for item in path.iterdir():
                if item.is_file():
                    img = load_single_image(item)
                    if img is not None:
                        images.append(img)

        if len(images) == 0:
            raise InputException('no suported images found')

        return tf.data.Dataset.from_tensor_slices(images).batch(1)
