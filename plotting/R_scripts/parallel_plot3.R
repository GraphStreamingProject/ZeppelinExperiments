library(tidyverse)
library("scales")

parallel <- read.csv("parallel_data3.csv")

ggplot() +
  geom_line(data = parallel, mapping = aes(color = mem, x = buffer_size, y = ingestion_rate), size = 0.5) +
  #scale_x_continuous(labels=comma, trans="log10") +
  labs(y=expression(paste("Ingestion rate (",~10^6," edges/s)")), x ="Gutter size") + 
  #guides(color=guide_legend(nrow=2,byrow=TRUE), linetype="none") +
  guides(linetype="none") +
  labs(color = "Sketch location") +
  theme(legend.position = c(0.45, -0.60), legend.direction = 'horizontal', plot.margin = grid::unit(c(1,0,2,0), 'lines')) +
  scale_color_hue(labels = c("Sketches on disk", "Sketches in RAM"))+
  theme(
    axis.title.y = element_text(hjust=0.8)
  ) +
  scale_y_continuous(labels=comma) 
ggsave(filename = "parallel3.png", width = 6, height = 2, unit = "in")
