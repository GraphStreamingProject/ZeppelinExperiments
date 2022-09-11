library(tidyverse)
library("scales")

speed <- read.csv("query_disk.csv")

ggplot() +
  geom_line(data = speed, mapping = aes(x = percent, y = flush+cc, color = system, linetype =system), size = 0.5) +
  labs(y=expression(paste("Query time (s)")), x ="percent of stream") +
  labs(color = "System\n") +
  scale_color_hue(labels = c("Aspen", "GraphZeppelin"))+
  scale_linetype_manual("", breaks=c("aspen_top", "stream_mem")
                        , values=c("dashed", "solid"))

ggsave(filename = "query_disk.png", width = 6, height = 2, unit = "in")
