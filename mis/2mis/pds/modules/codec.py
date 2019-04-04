#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  Bencode Codec
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""


class Bencodec:
    """Bencode Codec"""

    @staticmethod
    def encode(data):
        """Encode the data to bencode."""
        # integer
        if isinstance(data, int):
            return 'i{:d}e'.format(data)

        # string
        elif isinstance(data, str):
            return '{:d}:{}'.format(len(data), data)

        # list
        elif isinstance(data, list):
            contents = ''
            # encode each item in the list
            for item in data:
                contents += Bencodec.encode(item)

            return 'l{}e'.format(contents)

        # dictionary
        elif isinstance(data, dict):
            contents = ''
            for key in sorted(data):
                contents += '{}{}'.format(Bencodec.encode(key), Bencodec.encode(data[key]))

            return 'd{}e'.format(contents)

    @staticmethod
    def decode(data):
        """Decode the data from bencode."""

        def parse_head(head):
            """Parse the head of the data."""
            # must be a non-empty string
            if not isinstance(head, str) or len(head) == 0:
                return None, None

            # integer
            if head.startswith('i'):
                # find integer's end
                end = head.find('e')
                if end == -1:  # format error
                    return None, None

                num = head[1:end]   # integer part (strip the i<>e)
                rest = head[end+1:]  # rest of the string

                # try to convert the string(integer part) into an actual integer
                try:
                    return int(num), rest
                except Exception:
                    return None, None

            # string
            elif head[0].isdigit():
                pos = head.find(':')
                if pos == -1:  # format error
                    return None, None

                # try to convert the string length to an integer
                length = head[:pos]  # string length
                try:
                    length = int(length)
                except Exception:
                    return None, None

                # not enought characters
                if len(head[pos+1:]) < length:
                    return None, None

                string = head[pos+1:pos+1+length]  # actual string
                rest = head[pos+1+length:]         # rest of the data

                return string, rest

            # list
            elif head.startswith('l'):
                lst = []  # empty list
                head = head[1:]  # strip the first letter
                while len(head) > 0:
                    item, head = parse_head(head)
                    if item is None or head == '':  # an error occured or missing end of the list
                        return None, None
                    else:
                        lst.append(item)

                    if head.startswith('e'):  # end of the list
                        return lst, head[1:]

            # dictionary
            elif head.startswith('d'):
                dic = dict()
                head = head[1:]  # strip the first letter
                odd = True  # flag to indicate a key or a value of the key
                key = None  # key
                while len(head) > 0:
                    item, head = parse_head(head)
                    if item is None or head == '':  # an error occured or missing end of the dict
                        return None, None
                    else:
                        # just parsed a key
                        if odd:
                            if not isinstance(item, str):  # key has to be a string
                                return None, None

                            key = item
                        else:
                            dic[key] = item  # key and value both parsed already

                        # toggle flag
                        odd = not odd

                    if head.startswith('e'):  # end of the list
                        # parsed a key without the value -> error
                        if not odd:
                            return None, None
                        else:
                            return dic, head[1:]

            # unknown format
            else:
                return None, None

        # process the data
        decoded, data = parse_head(data)
        if data != '':  # something left unparsed -> invalid syntax
            return None

        # return the decoded data
        return decoded
