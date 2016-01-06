# DiscoverContest
This is a contest about data modeling from discover company.

It is easy to find a model to predict the future data, but calculating the unknown records accurately is difficult. It is hard for my laptop to collect a large number of items in the data.txt, so I designed a simple structure to store some filed and write them into other file named MoreSimple.db.    

I found that two factors affect the final results though analyzing the data.txt, the one is the weekday and the other is the day of a month. But rule of thumb, the holiday should be one of them as well. Relations between CallVolume and the holiday couldn’t be found beyond analyzed, I just provided a interface based on the holiday, but not make it true.  

In this Simple Prediction Model, the future result (callVolume, handlingTime ) of each day depends on average result on a daily base, the weight of every weekday and the weight every day of a month. It will be introduced in detail later.
