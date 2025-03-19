Error rate is the percentage off the succesfull received DCF time messages which differ less then 2 minutes from NTP compared to the total DCF messages the receiver should have received. 

Total messages is calculated based on the passed time since starting receiving times the DCF messages per minute (1 message)


Time errors are errors when the received time from DCF is more than 2 minutes different than NTP time. This is caused when the time information is mangled but the parity check was passed.

Times succesfull is when the received time is less then 2 minutes off from NTP time.