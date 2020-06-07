# R program to plot CSV results of 'bench_sum' program to a single image file

library(readr)
library(ggplot2)
library(reshape2)
library(scales)

scientific_10 <- function(x)
{
    parse(text = gsub("e", " %*% 10^", scales::scientific_format()(x)))
}

files = list.files(pattern = "*_all.csv", full.names = TRUE)

for (file in files)
{
    data = read_csv(file)
    transformed_data = melt(data, id = "quantity")

    p <- ggplot(transformed_data, aes(x = quantity, y = value, col = variable)) +
        geom_line() +
        geom_point() +
        scale_x_continuous(label = scientific_10) +
        scale_y_continuous(labels = comma) +
        labs(title = "Sum Array Of Bytes", x = "Array Length (Bytes)", y = "Average Time (Microseconds)") +
        theme(legend.title = element_blank()) + theme(plot.title = element_text(hjust = 0.5))

    file = gsub(".csv", ",jpg", file)

    ggsave(file, width = 10, height = 5, dpi = 100, units = "in", device = "jpg")
}
