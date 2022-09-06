library(tidyverse)
library("scales")

speed <- read.csv("speed_data.csv")

ggplot() +
  geom_line(data = speed, mapping = aes(x = nodes, y = ingestion_rate/10^6, color = alg, linetype = alg), size = 0.5) +
  scale_x_continuous(trans="log10") +
  labs(y=expression(paste("Ingestion rate (", ~10^6," edges/second)")), x ="number of nodes") + 
  scale_y_continuous(trans="log10") +
  labs(color = "System\n") +
  theme(legend.position = c(0.45, -0.23), legend.direction = 'horizontal', plot.margin = grid::unit(c(0,1,2,0), 'lines')) +
  guides(linetype="none") +
  scale_color_hue(labels = c("Aspen", "GraphZeppelin gutter tree", "GraphZeppelin leaf gutters", "Terrace"))+
  scale_linetype_manual("", breaks=c("aspen_lim_e3", "stream_mem", "terrace_lim", "stream_ext")
                        , values=c("dashed", "solid","longdash","dotdash"))
ggsave(filename = "speed.png", width = 6, height = 3.5, unit = "in")
