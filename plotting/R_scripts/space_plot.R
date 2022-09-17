library(tidyverse)
library("scales")


size <- read.csv("space_data.csv")

ggplot() +
  geom_line(data = size, mapping = aes(x=nodes, y=total_no_disk, color=alg, linetype=alg), size = 0.5) +
  ylab("maximum data structure size (GiB)") + xlab("number of nodes") +
  labs(color = "System\n", linetype = "System\n") +
  theme(legend.position = c(0.45, -0.20), legend.direction = 'horizontal', plot.margin = grid::unit(c(0,1,2,0), 'lines')) +
  guides(linetype="none") +
  scale_color_hue(labels = c("Aspen", "GraphZeppelin", "Terrace"))+
  scale_linetype_manual("", breaks=c("aspen_top", "stream_mem", "terrace")
                      , values=c("longdash", "solid","dashed"))
  
ggsave(filename = "size.png", width = 6, height = 4, unit = "in")
