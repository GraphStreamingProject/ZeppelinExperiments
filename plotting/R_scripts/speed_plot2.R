library(tidyverse)
library("scales")

speed <- read.csv("speed_data_unlim.csv")

ggplot() +
  geom_line(data = speed, mapping = aes(x = nodes, y = ingestion_rate/10^6, color = alg, linetype = alg), size = 0.5) +
  labs(y=expression(paste("Ingestion rate (", ~10^6," edges/s)")), x ="number of nodes") + 
  scale_y_continuous(trans="log10") +
  labs(color = "System\n") +
  theme(legend.position = c(0.45, -0.30), legend.direction = 'horizontal', plot.margin = grid::unit(c(0,1,2,0), 'lines')) +
  guides(linetype="none") +
  scale_color_hue(labels = c("GraphZeppelin Gutter Tree", "GraphZepplin leaf gutters", "Aspen")) + 
  scale_linetype_manual("", breaks=c("stream_mem", "stream_ext", "aspen_top")
                        , values=c("longdash", "solid", "dashed"))

ggsave(filename = "speed_unlim.png", width = 6, height = 3, unit = "in")
