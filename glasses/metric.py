# Copyright (c) 2020, Soohwan Kim. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import Levenshtein as Lev


class ErrorRate(object):
    """
    Provides inteface of error rate calcuation.
    Note:
        Do not use this class directly, use one of the sub classes.
    """

    def __init__(self) -> None:
        self.total_dist = 0.0
        self.total_length = 0.0

    def __call__(self, targets, y_hats):
        """ Calculating character error rate """
        dist, length = self._get_distance(targets, y_hats)
        self.total_dist += dist
        self.total_length += length
        return self.total_dist / self.total_length

    def metric(self, *args, **kwargs):
        raise NotImplementedError


class CharacterErrorRate(ErrorRate):
    """
    Computes the Character Error Rate, defined as the edit distance between the
    two provided sentences after tokenizing to characters.
    """
    def __init__(self, ):
        super(CharacterErrorRate, self).__init__()

    def metric(self, s1: str, s2: str):
        """
        Computes the Character Error Rate, defined as the edit distance between the
        two provided sentences after tokenizing to characters.
        Arguments:
            s1 (string): space-separated sentence
            s2 (string): space-separated sentence
        """
        s1 = s1.replace(' ', '')
        s2 = s2.replace(' ', '')
        
        ## rule update 2021.08.04
        s1 = s1.replace(',', '')
        s2 = s2.replace(',', '')
        
        ## rule update 2021.08.31
        s1 = s1.replace('.','')
        s1 = s1.replace(',','')
        s1 = s1.replace('?','')
        s1 = s1.replace('!','')
        s1 = s1.replace('“','')
        s1 = s1.replace('”','')
        s1 = s1.replace("'","")
        s1 = s1.replace('"','')
        s1 = s1.replace('‘','')
        s1 = s1.replace('’','')
        

        s2 = s2.replace('.','')
        s2 = s2.replace(',','')
        s2 = s2.replace('?','')
        s2 = s2.replace('!','')
        s2 = s2.replace('“','')
        s2 = s2.replace('”','')
        s2 = s2.replace("'","")
        s2 = s2.replace('"','')
        s2 = s2.replace('‘','')
        s2 = s2.replace('’','')

        # if '_' in sentence, means subword-unit, delete '_'
        if '_' in s1:
            s1 = s1.replace('_', '')

        if '_' in s2:
            s2 = s2.replace('_', '')

        dist = Lev.distance(s2, s1)
        length = len(s1.replace(' ', ''))

        return dist, length


class WordErrorRate(ErrorRate):
    """
    Computes the Word Error Rate, defined as the edit distance between the
    two provided sentences after tokenizing to words.
    """
    def __init__(self,):
        super(WordErrorRate, self).__init__()

    def metric(self, s1, s2):
        """
        Computes the Word Error Rate, defined as the edit distance between the
        two provided sentences after tokenizing to words.
        Arguments:
            s1 (string): space-separated sentence
            s2 (string): space-separated sentence
        """

        # build mapping of words to integers
        b = set(s1.split() + s2.split())
        word2char = dict(zip(b, range(len(b))))

        # map the words to a char array (Levenshtein packages only accepts
        # strings)
        w1 = [chr(word2char[w]) for w in s1.split()]
        w2 = [chr(word2char[w]) for w in s2.split()]

        return Lev.distance(''.join(w1), ''.join(w2))