#!/usr/bin/env python
# encoding: utf-8

# Original version from https://gist.github.com/yanofsky/5436496

import tweepy
import sys

#Twitter API credentials
from tweet_dumper_config import *

def quote_for_c(s):
    return s.replace("\"", "\\\"").replace("\n","\\n")

def get_all_tweets(screen_name):
        #Twitter only allows access to a users most recent 3240 tweets with this method

        #authorize twitter, initialize tweepy
        auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
        auth.set_access_token(access_key, access_secret)
        api = tweepy.API(auth)

        #initialize a list to hold all the tweepy Tweets
        alltweets = []

        #make initial request for most recent tweets (200 is the maximum allowed count)
        new_tweets = api.user_timeline(screen_name = screen_name,count=200)

        #save most recent tweets
        alltweets.extend(new_tweets)

        if len(alltweets) == 0:
            print("No tweets?")
            return

        #save the id of the oldest tweet less one
        oldest = alltweets[-1].id - 1

        #keep grabbing tweets until there are no tweets left to grab
        while len(new_tweets) > 0:
                print "getting tweets before %s" % (oldest)

                #all subsiquent requests use the max_id param to prevent duplicates
                new_tweets = api.user_timeline(screen_name = screen_name,count=200,max_id=oldest)

                #save most recent tweets
                alltweets.extend(new_tweets)

                #update the id of the oldest tweet less one
                oldest = alltweets[-1].id - 1

                print "...%s tweets downloaded so far" % (len(alltweets))

        #transform the tweepy tweets into a 2D array that will populate the csv
        outtweets = [[tweet.id_str, tweet.created_at, tweet.text.encode("utf-8")] for tweet in alltweets]

        #write the csv
        with open('%s_tweets.inc' % screen_name, 'wb') as f:
            for tweet in alltweets:
                f.write("\"%s\\n - @%s\",\n" % (quote_for_c(tweet.text.encode("utf-8")), quote_for_c(screen_name)))

if __name__ == '__main__':
    #["picardtips", "laforgetips", "worfemail", "data_tips", "locutusTips", "wesleytips"]:
    for user in sys.argv[1:]:
        print("Fetching tweets for %s..." % user )
        get_all_tweets(user)
