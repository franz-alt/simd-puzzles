# R program to plot CSV results of a benchmark program to multiple image files

library(readr)
library(ggplot2)
library(reshape2)

files = list.files(pattern = "*.csv", full.names = TRUE)

for (file in files)
{
    # in case a '*_all.csv' exists, ignore it
    if (grepl("_all", file, fixed = TRUE))
    {
        next
    }

    data = read_csv(file)
    transformed_data = melt(data, id.vars = "Procedure")

    p <- ggplot(transformed_data, aes(x = Procedure, y = value, fill = variable)) + geom_bar(stat = "identity", fill = "steelblue", position = position_dodge())
    p + coord_flip() + labs(y = "Average Time (Microseconds)")

    file = gsub(".csv", ".jpg", file)

    ggsave(file, width = 10, height = 3, dpi = 75, units = "in", device = "jpg")
}
