library(tidyverse)
library("scales")

speed <- read.csv("speed_data_unlim.csv")

ggplot() +
  geom_line(data = speed, mapping = aes(x = nodes, y = ingestion_rate/10^6, color = alg, linetype = alg), size = 0.5) +
  #scale_x_continuous(trans="log10") +
  labs(y=expression(paste("Ingestion rate (", ~10^6," edges/s)")), x ="number of nodes") + 
  scale_y_continuous(trans="log10") +
  #coord_cartesian(ylim = c(0, 3))
  #labs(title = "Ingestion Rate\n", color = "System\n") +
  labs(color = "System\n") +
  theme(legend.position = c(0.45, -0.30), legend.direction = 'horizontal', plot.margin = grid::unit(c(0,1,2,0), 'lines')) +
  #guides(color=guide_legend(nrow=2,byrow=TRUE), linetype="none") +
  guides(linetype="none") +
  #guides(linetype="none") +
  scale_color_hue(labels = c("Aspen", "GraphZeppelin Gutter Tree", "GraphZepplin leaf gutters")) + 
  scale_linetype_manual("", breaks=c("aspen_top", "stream_mem", "stream_ext")
                        , values=c("dashed", "longdash","solid"))

ggsave(filename = "speed_unlim.png", width = 6, height = 3, unit = "in")
