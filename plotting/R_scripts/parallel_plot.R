library(tidyverse)
library("scales")

parallel <- read.csv("parallel_data.csv")

ggplot() +
  geom_line(data = parallel, mapping = aes(x = threads, y = insertion_rate), size = 0.5) +
  #scale_x_continuous(labels=comma, trans="log10") +
  labs(y=expression(paste("Ingestion rate (", ~10^6," edges/second)")), x ="Number of threads") +
  guides(color=guide_legend(nrow=2,byrow=TRUE), linetype="none") +
  scale_y_continuous(labels=comma) 
ggsave(filename = "parallel.png", width = 6, height = 3, unit = "in")
